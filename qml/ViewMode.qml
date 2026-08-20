import QtQuick
import QtQuick.Controls.Basic
import MDReader

Item {
    id: root

    property string source: ""
    property int fontSize: AppSettings.viewFontSize

    property url documentUrl: ""

    property bool claimFocus: true

    readonly property alias textItem: doc
    readonly property real scrollFraction: flick.contentHeight > flick.height
        ? flick.contentY / (flick.contentHeight - flick.height)
        : 0

    readonly property real viewportY: flick.contentY

    property int matchStart: -1
    property int matchLength: 0

    function highlightMatch(position, length) {
        root.matchStart = position;
        root.matchLength = length;
    }

    function clearMatch() {
        root.matchStart = -1;
        root.matchLength = 0;
    }

    property int currentHeading: -1

    property bool _stale: true

    onSourceChanged: {
        _stale = true;
        if (visible)
            applySource();
    }

    onVisibleChanged: {
        if (visible && _stale)
            applySource();
    }

    Component.onCompleted: {
        if (visible)
            applySource();
    }

    property string _appliedSource: " "

    property bool rebuilding: false

    property real _restoreY: 0

    function _restoreScroll() {
        if (!isFinite(root._restoreY) || !isFinite(flick.contentHeight) || !isFinite(flick.height))
            return;
        const limit = Math.max(0, flick.contentHeight - flick.height);
        flick.contentY = Math.max(0, Math.min(root._restoreY, limit));
    }

    onDocumentUrlChanged: {
        _appliedSource = " ";
        if (visible)
            applySource();
        else
            _stale = true;
    }

    function applySource() {
        if (_appliedSource !== root.source) {
            if (root.rebuilding) {
                retryTimer.restart();
                return;
            }
            styler.baseUrl = root.documentUrl;

            _restoreY = flick.contentY;
            rebuilding = true;

            const started = Date.now();
            const prepared = styler.preprocess(root.source);

            doc.text = prepared;
            styler.refreshViewportCulling();
            styler.restyle();

            _appliedSource = root.source;
            _restoreScroll();
            settleTimer.restart();

            root.lastRebuildMs = Date.now() - started;
        }
        _stale = false;
        headingTracker.restart();
    }

    Timer {
        id: retryTimer
        interval: 60
        onTriggered: root.applySource()
    }

    property int lastRebuildMs: 0

    Timer {
        id: settleTimer
        interval: 40
        onTriggered: {
            root._restoreScroll();
            root.rebuilding = false;
            headingTracker.restart();
        }
    }

    Timer {
        id: rebuildTimer
        interval: 0
        onTriggered: {
            root._appliedSource = " ";
            root.applySource();
        }
    }

    readonly property int headingScrollMargin: 14

    function followLink(link) {
        const anchor = root.anchorIn(link);
        if (anchor.length === 0) {
            Qt.openUrlExternally(link);
            return;
        }

        const top = styler.anchorTop(anchor);
        if (top >= 0)
            scrollTo(flick.pad + top - headingScrollMargin, true);
    }

    function anchorIn(link) {
        const text = link.toString();
        if (text.startsWith("#"))
            return text;

        const hash = text.indexOf("#");
        const self = root.documentUrl.toString();
        if (hash > 0 && self.length > 0 && text.substring(0, hash) === self)
            return text.substring(hash);

        return "";
    }

    function scrollToHeading(index) {
        const top = styler.headingTop(index);
        if (top < 0)
            return;
        scrollTo(flick.pad + top - headingScrollMargin, true);
    }

    Timer {
        id: headingTracker
        interval: 70
        onTriggered: root.currentHeading =
            styler.headingIndexAt(flick.contentY - flick.pad + root.headingScrollMargin + 2)
    }

    Connections {
        target: flick
        function onContentYChanged() { headingTracker.restart(); }
        function onContentHeightChanged() { headingTracker.restart(); }
    }


    function scrollTo(y, animated) {
        if (!isFinite(y) || !isFinite(flick.contentHeight) || !isFinite(flick.height))
            return;

        const target = Math.max(0, Math.min(y, Math.max(0, flick.contentHeight - flick.height)));
        if (animated) {
            scrollAnimation.to = target;
            scrollAnimation.restart();
        } else {
            flick.contentY = target;
        }
    }

    function setScrollFraction(fraction) {
        if (!isFinite(fraction) || !isFinite(flick.contentHeight) || !isFinite(flick.height))
            return;
        const span = Math.max(0, flick.contentHeight - flick.height);
        flick.contentY = span * Math.max(0, Math.min(1, fraction));
    }

    function ensureVisible(position) {
        const rect = doc.positionToRectangle(position);
        if (!rect || !isFinite(rect.y) || !isFinite(rect.height))
            return;

        const top = flick.pad + rect.y;
        const bottom = top + rect.height;
        const margin = 48;

        if (top < flick.contentY + margin)
            scrollTo(top - margin, false);
        else if (bottom > flick.contentY + flick.height - margin)
            scrollTo(bottom - flick.height + margin, false);
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.mdBackground
    }

    Flickable {
        id: flick

        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: doc.height + 2 * pad
        boundsBehavior: Flickable.StopAtBounds
        flickDeceleration: 4000
        focus: root.claimFocus

        readonly property real pad: Math.max(16, Math.min(Theme.pagePadding, width * 0.07))

        NumberAnimation {
            id: scrollAnimation
            target: flick
            property: "contentY"
            duration: 260
            easing.type: Easing.OutCubic
        }

        ScrollBar.vertical: SlimScrollBar {}

        Repeater {
            model: styler.codeRegions

            delegate: Rectangle {
                required property var modelData

                x: doc.x
                y: doc.y + modelData.top
                width: doc.width
                height: modelData.height

                radius: Theme.radius
                color: Theme.mdCodeBg
                border.width: 1
                border.color: Theme.mdTableBorder
            }
        }

        Rectangle {
            id: matchHighlight

            readonly property rect head: root.matchStart >= 0
                ? doc.positionToRectangle(root.matchStart) : Qt.rect(0, 0, 0, 0)
            readonly property rect tail: root.matchStart >= 0
                ? doc.positionToRectangle(root.matchStart + root.matchLength)
                : Qt.rect(0, 0, 0, 0)

            visible: root.matchStart >= 0 && root.matchLength > 0
            x: doc.x + head.x
            y: doc.y + head.y
            width: Math.max(2, tail.y > head.y ? doc.width - head.x : tail.x - head.x)
            height: Math.max(2, head.height)
            radius: 2
            color: Theme.selection
        }

        TextEdit {
            id: doc

            x: Math.max(flick.pad, (flick.width - width) / 2)
            y: flick.pad
            width: Math.min(Theme.readingWidth, flick.width - 2 * flick.pad)

            readOnly: true
            selectByMouse: true
            selectByKeyboard: true
            persistentSelection: true
            textFormat: TextEdit.MarkdownText

            renderType: Text.NativeRendering
            wrapMode: TextEdit.Wrap
            activeFocusOnPress: false

            font.family: Theme.readingFont
            font.pixelSize: root.fontSize
            color: Theme.mdText
            selectionColor: Theme.selection
            selectedTextColor: Theme.mdText

            onLinkActivated: (link) => root.followLink(link)

            MarkdownDocument {
                id: styler
                document: doc.textDocument
                lineHeight: Theme.readingLineHeight
                codeFont: Theme.editorFont

                headingColor: Theme.mdHeading
                codeColor: Theme.mdCode
                codeBackground: Theme.mdCodeBg
                inlineCodeColor: Theme.mdInlineCode
                inlineCodeBackground: Theme.mdInlineCodeBg
                linkColor: Theme.mdLink
                quoteColor: Theme.mdQuote
                boldColor: Theme.mdBold
                italicColor: Theme.mdItalic
                markerColor: Theme.mdMarker
                ruleColor: Theme.mdRule
                highlightBackground: Theme.mdMarkBg
                codeKeywordColor: Theme.mdCodeKeyword
                codeTypeColor: Theme.mdCodeType
                codeStringColor: Theme.mdCodeString
                codeNumberColor: Theme.mdCodeNumber
                codeCommentColor: Theme.mdCodeComment
                codeFunctionColor: Theme.mdCodeFunction
                codeVariableColor: Theme.mdCodeVariable

                checkboxFont: Theme.iconFont
                checkedGlyph: Theme.glyphChecked
                uncheckedGlyph: Theme.glyphUnchecked
                checkedColor: Theme.mdChecked
                uncheckedColor: Theme.mdUnchecked

                tableBorderColor: Theme.mdTableBorder
                tableHeaderBackground: Theme.mdTableHead
                tableStripeBackground: Theme.mdTableStripe

                onDocumentChanged: restyle()
                onStyleChanged: rebuildTimer.restart()
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                hoverEnabled: true
                cursorShape: doc.hoveredLink.length > 0 ? Qt.PointingHandCursor : Qt.IBeamCursor
            }

            TapHandler {
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onPressedChanged: {
                    if (pressed)
                        flick.forceActiveFocus();
                }

                onTapped: (point, button) => {
                    if (button === Qt.RightButton)
                        contextMenu.popupAt(doc, point.position);
                }
            }
        }

        Repeater {
            model: styler.codeRegions

            delegate: Item {
                id: codeHeader

                required property var modelData

                x: doc.x
                y: doc.y + modelData.top
                width: doc.width
                height: modelData.headerHeight

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 14
                    anchors.verticalCenter: parent.verticalCenter
                    visible: text.length > 0
                    text: codeHeader.modelData.language
                    font.family: Theme.uiFont
                    font.pixelSize: 11
                    color: Theme.textFaint
                }

                Rectangle {
                    id: copyButton

                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    width: 34
                    height: 26
                    radius: Theme.radiusSmall

                    color: copyArea.pressed ? Theme.overlay
                         : (copyArea.containsMouse ? Theme.surfaceAlt : "transparent")
                    border.width: copyArea.containsMouse ? 1 : 0
                    border.color: Theme.border
                    opacity: copyArea.containsMouse || copied.running ? 1 : 0.45

                    Behavior on opacity {
                        NumberAnimation { duration: Theme.animFast }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: copied.running ? Theme.iconCheck : Theme.iconCopy
                        font.family: Theme.iconFont
                        font.pixelSize: 14
                        color: copied.running ? Theme.accent : Theme.textMuted
                    }

                    Timer {
                        id: copied
                        interval: 1200
                    }

                    MouseArea {
                        id: copyArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            styler.copyToClipboard(codeHeader.modelData.text);
                            copied.restart();
                        }
                    }
                }
            }
        }

        Keys.onPressed: (event) => {
            if (event.matches(StandardKey.Copy)) {
                doc.copy();
                event.accepted = true;
                return;
            }
            if (event.matches(StandardKey.SelectAll)) {
                doc.selectAll();
                event.accepted = true;
                return;
            }

            const page = flick.height * 0.9;
            switch (event.key) {
            case Qt.Key_PageDown:
                root.scrollTo(flick.contentY + page, true);
                event.accepted = true;
                break;
            case Qt.Key_PageUp:
                root.scrollTo(flick.contentY - page, true);
                event.accepted = true;
                break;
            case Qt.Key_Home:
                root.scrollTo(0, true);
                event.accepted = true;
                break;
            case Qt.Key_End:
                root.scrollTo(flick.contentHeight, true);
                event.accepted = true;
                break;
            case Qt.Key_Down:
                root.scrollTo(flick.contentY + 60, false);
                event.accepted = true;
                break;
            case Qt.Key_Up:
                root.scrollTo(flick.contentY - 60, false);
                event.accepted = true;
                break;
            }
        }
    }

    AutoScroller {
        target: flick
    }

    TextContextMenu {
        id: contextMenu
        target: doc
        onClosed: flick.forceActiveFocus()
    }

    Column {
        anchors.centerIn: parent
        spacing: 10
        visible: root.source.length === 0
        opacity: 0.9

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: Theme.iconOpen
            font.family: Theme.iconFont
            font.pixelSize: 34
            color: Theme.textFaint
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Open a markdown file")
            font.family: Theme.uiFont
            font.pixelSize: 14
            color: Theme.textMuted
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Ctrl+O, or drop a file onto the window")
            font.family: Theme.uiFont
            font.pixelSize: 12
            color: Theme.textFaint
        }
    }
}
