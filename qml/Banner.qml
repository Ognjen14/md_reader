import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader

Rectangle {
    id: root

    property string message: ""
    property string actionText: ""
    property bool active: false

    signal actionTriggered()
    signal dismissed()

    visible: height > 0
    implicitHeight: active ? 34 : 0
    clip: true
    color: Theme.dark ? Qt.darker(Theme.warning, 3.4) : Qt.lighter(Theme.warning, 1.85)

    Behavior on implicitHeight {
        NumberAnimation { duration: Theme.animNormal; easing.type: Easing.OutCubic }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.border
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 6
        spacing: 8

        Text {
            text: Theme.iconWarning
            font.family: Theme.iconFont
            font.pixelSize: 12
            color: Theme.warning
        }

        Text {
            Layout.fillWidth: true
            text: root.message
            elide: Text.ElideRight
            font.family: Theme.uiFont
            font.pixelSize: 12
            color: Theme.text
        }

        AppButton {
            visible: root.actionText.length > 0
            text: root.actionText
            implicitHeight: 24
            implicitWidth: 74
            focusPolicy: Qt.NoFocus
            onClicked: root.actionTriggered()
        }

        IconButton {
            glyph: Theme.iconClose
            tip: qsTr("Dismiss")
            implicitWidth: 26
            implicitHeight: 24
            onClicked: root.dismissed()
        }
    }
}
