import QtQuick
import QtQuick.Layouts
import MDReader

Rectangle {
    id: root

    property int words: 0
    property int characters: 0
    property int line: 1
    property int column: 1
    property bool showCursor: false

    implicitHeight: Theme.statusHeight
    color: Theme.surface

    Rectangle {
        anchors.top: parent.top
        width: parent.width
        height: 1
        color: Theme.border
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 16

        Item { Layout.fillWidth: true; Layout.preferredHeight: 1 }

        Text {
            visible: root.showCursor
            font.family: Theme.uiFont
            font.pixelSize: 11
            color: Theme.textFaint
            text: qsTr("Ln %1, Col %2").arg(root.line).arg(root.column)
        }

        Text {
            font.family: Theme.uiFont
            font.pixelSize: 11
            color: Theme.textFaint
            text: root.words === 1 ? qsTr("1 word")
                                   : qsTr("%1 words").arg(root.words)
        }

        Text {
            font.family: Theme.uiFont
            font.pixelSize: 11
            color: Theme.textFaint
            text: qsTr("%1 chars").arg(root.characters)
        }
    }
}
