import QtQuick
import QtQuick.Controls.Basic
import MDReader

Item {
    id: root

    property string source: ""
    property int fontSize: AppSettings.editorFontSize

    readonly property alias textArea: editor
    readonly property int cursorLine: cursorPos.x
    readonly property int cursorColumn: cursorPos.y

    readonly property real scrollFraction: editFlick.contentHeight > editFlick.height
        ? editFlick.contentY / (editFlick.contentHeight - editFlick.height)
        : 0
    readonly property real viewportY: editFlick.contentY

    function setScrollFraction(fraction) {
        if (!isFinite(fraction) || !isFinite(editFlick.contentHeight)
            || !isFinite(editFlick.height)) {
            return;
        }
        const span = Math.max(0, editFlick.contentHeight - editFlick.height);
        editFlick.contentY = span * Math.max(0, Math.min(1, fraction));
    }

    signal sourceEdited(string text)

    onSourceChanged: {
        if (editor.text !== root.source) {
            editor.text = root.source;
            docInfo.refreshViewportCulling();
        }
    }

    Component.onCompleted: {
        if (editor.text !== root.source)
            editor.text = root.source;
    }

    function focusEditor() {
        editor.forceActiveFocus();
    }

    function lineAt(position) {
        return docInfo.lineColumnAt(position).x;
    }

    function positionForLine(line) {
        return docInfo.positionForLine(line);
    }

    function goToLine(line) {
        editor.cursorPosition = docInfo.positionForLine(line);
        editor.forceActiveFocus();
    }

    property Item caretItem: null

    function restoreCaretVisibility() {
        if (caretItem)
            caretItem.visible = editor.cursorVisible;
    }

    property point cursorPos: Qt.point(1, 1)

    Timer {
        id: cursorTimer
        interval: 60
        onTriggered: root.cursorPos = docInfo.lineColumnAt(editor.cursorPosition)
    }

    MarkdownDocument {
        id: docInfo
        document: editor.textDocument
    }

    MarkdownHighlighter {
        document: editor.textDocument
        baseFontSize: root.fontSize

        headingColor: Theme.mdHeading
        boldColor: Theme.mdBold
        italicColor: Theme.mdItalic
        codeColor: Theme.mdCode
        codeBackground: Theme.mdCodeBg
        inlineCodeColor: Theme.mdInlineCode
        inlineCodeBackground: Theme.mdInlineCodeBg
        linkColor: Theme.mdLink
        markerColor: Theme.mdMarker
        quoteColor: Theme.mdQuote
        delimiterColor: Theme.mdDelimiter
        ruleColor: Theme.mdRule
        highlightBackground: Theme.mdMarkBg
        codeKeywordColor: Theme.mdCodeKeyword
        codeTypeColor: Theme.mdCodeType
        codeStringColor: Theme.mdCodeString
        codeNumberColor: Theme.mdCodeNumber
        codeCommentColor: Theme.mdCodeComment
        codeFunctionColor: Theme.mdCodeFunction
        codeVariableColor: Theme.mdCodeVariable
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.mdBackground
    }

    Flickable {
        id: editFlick

        anchors.fill: parent
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: SlimScrollBar {}

        TextArea.flickable: TextArea {
            id: editor

            wrapMode: TextArea.Wrap
            selectByMouse: true
            persistentSelection: true
            textFormat: TextEdit.PlainText

            renderType: Text.NativeRendering

            leftPadding: 22
            rightPadding: 22
            topPadding: 18
            bottomPadding: 60

            font.family: Theme.editorFont
            font.pointSize: root.fontSize
            color: Theme.mdText
            selectionColor: Theme.selection
            selectedTextColor: Theme.mdText

            cursorDelegate: Rectangle {
                id: caret

                width: 1
                color: Theme.mdText

                Component.onCompleted: root.caretItem = caret
                visible: editor.cursorVisible

                Connections {
                    target: editor
                    function onCursorPositionChanged() {
                        caret.opacity = 1;
                        blink.restart();
                    }
                }

                Timer {
                    id: blink

                    running: editor.activeFocus && !editor.readOnly
                             && Qt.styleHints.cursorFlashTime > 0
                    repeat: true
                    interval: Qt.styleHints.cursorFlashTime / 2
                    onTriggered: caret.opacity = caret.opacity > 0 ? 0 : 1
                    onRunningChanged: caret.opacity = 1
                }
            }

            background: null

            onTextChanged: root.sourceEdited(text)
            onCursorPositionChanged: cursorTimer.restart()

            onCursorRectangleChanged: root.restoreCaretVisibility()

            function handleReturn(event) {
                const text = editor.text;
                const pos = editor.selectionStart;
                if (editor.selectionStart !== editor.selectionEnd) {
                    event.accepted = false;
                    return;
                }

                const lineStart = text.lastIndexOf("\n", pos - 1) + 1;
                const line = text.substring(lineStart, pos);
                const match = /^(\s*)([-*+]|\d+[.)])([ \t]+)(\[[ xX]\][ \t]+)?/.exec(line);
                if (!match) {
                    event.accepted = false;
                    return;
                }

                const remainder = line.substring(match[0].length);
                if (remainder.trim().length === 0) {
                    editor.remove(lineStart, pos);
                    event.accepted = true;
                    return;
                }

                let marker = match[2];
                if (/^\d+[.)]$/.test(marker))
                    marker = (parseInt(marker, 10) + 1) + marker.charAt(marker.length - 1);

                const task = match[4] ? "[ ] " : "";
                editor.insert(pos, "\n" + match[1] + marker + match[3] + task);
                event.accepted = true;
            }

            Keys.onReturnPressed: (event) => editor.handleReturn(event)
            Keys.onEnterPressed: (event) => editor.handleReturn(event)

            function indentUnit() {
                return " ".repeat(AppSettings.tabSpaces);
            }

            function shiftSelection(outdent) {
                const text = editor.text;
                const selStart = editor.selectionStart;
                const selEnd = editor.selectionEnd;
                const blockStart = text.lastIndexOf("\n", selStart - 1) + 1;
                let blockEnd = text.indexOf("\n", selEnd);
                if (blockEnd === -1)
                    blockEnd = text.length;

                const unit = editor.indentUnit();
                const lines = text.substring(blockStart, blockEnd).split("\n");
                const shifted = lines.map(function (line) {
                    if (!outdent)
                        return unit + line;
                    if (line.startsWith("\t"))
                        return line.substring(1);
                    let strip = 0;
                    while (strip < unit.length && line.charAt(strip) === " ")
                        ++strip;
                    return line.substring(strip);
                });

                const replacement = shifted.join("\n");
                editor.remove(blockStart, blockEnd);
                editor.insert(blockStart, replacement);
                editor.select(blockStart, blockStart + replacement.length);
            }

            TapHandler {
                acceptedButtons: Qt.RightButton
                onTapped: (point, button) => {
                    editor.forceActiveFocus();
                    contextMenu.popupAt(editor, point.position);
                }
            }

            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Tab) {
                    if (editor.selectionStart !== editor.selectionEnd)
                        editor.shiftSelection(false);
                    else
                        editor.insert(editor.cursorPosition, editor.indentUnit());
                    event.accepted = true;
                } else if (event.key === Qt.Key_Backtab) {
                    editor.shiftSelection(true);
                    event.accepted = true;
                }
            }
        }
    }

    AutoScroller {
        target: editFlick
    }

    TextContextMenu {
        id: contextMenu
        target: editor
        onClosed: editor.forceActiveFocus()
    }
}
