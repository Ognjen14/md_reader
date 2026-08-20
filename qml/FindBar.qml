import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader

Rectangle {
    id: root

    property alias term: field.text
    property alias replacement: replaceField.text
    property bool active: false
    property bool canReplace: false
    property int matchCount: 0
    property int currentMatch: 0

    signal findNext()
    signal findPrevious()
    signal replaceOne()
    signal replaceAll()
    signal closed()

    readonly property int rowHeight: 38

    visible: height > 0
    implicitHeight: active ? (canReplace ? 2 * rowHeight - 6 : rowHeight) : 0
    clip: true
    color: Theme.surface

    Behavior on implicitHeight {
        NumberAnimation { duration: Theme.animFast; easing.type: Easing.OutCubic }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.border
    }

    function focusField() {
        field.forceActiveFocus();
        field.selectAll();
    }

    function focusReplaceField() {
        replaceField.forceActiveFocus();
        replaceField.selectAll();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 6
        anchors.topMargin: 6
        spacing: 6

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            spacing: 6

            Text {
                Layout.preferredWidth: 16
                text: Theme.iconFind
                font.family: Theme.iconFont
                font.pixelSize: 13
                color: Theme.textMuted
            }

            Rectangle {
                Layout.preferredWidth: 260
                Layout.preferredHeight: 26
                radius: Theme.radiusSmall
                color: Theme.background
                border.width: 1
                border.color: field.activeFocus ? Theme.accent : Theme.border

                TextInput {
                    id: field

                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    verticalAlignment: TextInput.AlignVCenter
                    clip: true
                    selectByMouse: true

                    font.family: Theme.uiFont
                    font.pixelSize: 12
                    color: Theme.text
                    selectionColor: Theme.selection
                    selectedTextColor: Theme.textStrong

                    Keys.onReturnPressed: (event) => {
                        if (event.modifiers & Qt.ShiftModifier)
                            root.findPrevious();
                        else
                            root.findNext();
                        event.accepted = true;
                    }
                    Keys.onEscapePressed: (event) => {
                        root.closed();
                        event.accepted = true;
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        visible: field.text.length === 0
                        text: qsTr("Find")
                        font: field.font
                        color: Theme.textFaint
                    }
                }
            }

            Text {
                Layout.preferredWidth: 78
                font.family: Theme.uiFont
                font.pixelSize: 11
                color: root.term.length > 0 && root.matchCount === 0 ? Theme.danger
                                                                     : Theme.textFaint
                text: {
                    if (root.term.length === 0)
                        return "";
                    if (root.matchCount === 0)
                        return qsTr("No results");
                    return qsTr("%1 of %2").arg(root.currentMatch).arg(root.matchCount);
                }
            }

            IconButton {
                glyph: Theme.iconChevronUp
                tip: qsTr("Previous (Shift+Enter)")
                enabled: root.matchCount > 0
                onClicked: root.findPrevious()
            }

            IconButton {
                glyph: Theme.iconChevronDown
                tip: qsTr("Next (Enter)")
                enabled: root.matchCount > 0
                onClicked: root.findNext()
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 1 }

            IconButton {
                glyph: Theme.iconClose
                tip: qsTr("Close (Esc)")
                onClicked: root.closed()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            visible: root.canReplace
            spacing: 6

            Item { Layout.preferredWidth: 16; Layout.preferredHeight: 1 }

            Rectangle {
                Layout.preferredWidth: 260
                Layout.preferredHeight: 26
                radius: Theme.radiusSmall
                color: Theme.background
                border.width: 1
                border.color: replaceField.activeFocus ? Theme.accent : Theme.border

                TextInput {
                    id: replaceField

                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    verticalAlignment: TextInput.AlignVCenter
                    clip: true
                    selectByMouse: true

                    font.family: Theme.uiFont
                    font.pixelSize: 12
                    color: Theme.text
                    selectionColor: Theme.selection
                    selectedTextColor: Theme.textStrong

                    Keys.onReturnPressed: (event) => {
                        root.replaceOne();
                        event.accepted = true;
                    }
                    Keys.onEscapePressed: (event) => {
                        root.closed();
                        event.accepted = true;
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        visible: replaceField.text.length === 0
                        text: qsTr("Replace with")
                        font: replaceField.font
                        color: Theme.textFaint
                    }
                }
            }

            Rectangle {
                id: replaceButton

                Layout.preferredWidth: replaceLabel.implicitWidth + 20
                Layout.preferredHeight: 24
                radius: Theme.radiusSmall
                enabled: root.matchCount > 0
                color: !enabled ? "transparent"
                     : (replaceArea.pressed ? Theme.overlay
                     : (replaceArea.containsMouse ? Theme.surfaceAlt : "transparent"))
                border.width: 1
                border.color: enabled ? Theme.border : "transparent"

                Text {
                    id: replaceLabel
                    anchors.centerIn: parent
                    text: qsTr("Replace")
                    font.family: Theme.uiFont
                    font.pixelSize: 11
                    color: replaceButton.enabled ? Theme.text : Theme.textFaint
                }

                MouseArea {
                    id: replaceArea
                    anchors.fill: parent
                    enabled: replaceButton.enabled
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.replaceOne()
                }
            }

            Rectangle {
                id: replaceAllButton

                Layout.preferredWidth: replaceAllLabel.implicitWidth + 20
                Layout.preferredHeight: 24
                radius: Theme.radiusSmall
                enabled: root.matchCount > 0
                color: !enabled ? "transparent"
                     : (replaceAllArea.pressed ? Theme.overlay
                     : (replaceAllArea.containsMouse ? Theme.surfaceAlt : "transparent"))
                border.width: 1
                border.color: enabled ? Theme.border : "transparent"

                Text {
                    id: replaceAllLabel
                    anchors.centerIn: parent
                    text: qsTr("Replace all")
                    font.family: Theme.uiFont
                    font.pixelSize: 11
                    color: replaceAllButton.enabled ? Theme.text : Theme.textFaint
                }

                MouseArea {
                    id: replaceAllArea
                    anchors.fill: parent
                    enabled: replaceAllButton.enabled
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.replaceAll()
                }
            }

            Item { Layout.fillWidth: true; Layout.preferredHeight: 1 }
        }
    }
}
