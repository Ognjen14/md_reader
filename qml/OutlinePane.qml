import QtQuick
import QtQuick.Controls.Basic
import MDReader

Rectangle {
    id: root

    property alias model: list.model
    property int currentIndex: -1

    signal activated(int index)

    color: Theme.surface
    clip: true

    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: Theme.border
    }

    onCurrentIndexChanged: {
        if (currentIndex >= 0 && !list.moving)
            list.positionViewAtIndex(currentIndex, ListView.Contain);
    }

    Text {
        id: heading
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 14
        anchors.topMargin: 14
        height: 20
        text: qsTr("OUTLINE")
        font.family: Theme.uiFont
        font.pixelSize: 10
        font.letterSpacing: 1.1
        font.weight: Font.DemiBold
        color: Theme.textFaint
    }

    ListView {
        id: list

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: heading.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: 4
        anchors.bottomMargin: 8
        anchors.rightMargin: 1

        clip: true
        boundsBehavior: Flickable.StopAtBounds
        currentIndex: root.currentIndex
        highlightMoveDuration: 0

        ScrollBar.vertical: SlimScrollBar {}

        delegate: Item {
            id: entry

            required property int index
            required property string title
            required property int level
            required property int depth

            width: list.width
            height: label.implicitHeight + 10

            readonly property bool current: entry.index === root.currentIndex

            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 8
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                radius: Theme.radiusSmall
                color: entry.current ? Theme.accentSoft
                                     : (mouse.containsMouse ? Theme.surfaceAlt : "transparent")

                Behavior on color {
                    ColorAnimation { duration: Theme.animFast }
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.leftMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                width: 2
                height: parent.height - 8
                radius: 1
                color: Theme.accent
                opacity: entry.current ? 1 : 0

                Behavior on opacity {
                    NumberAnimation { duration: Theme.animFast }
                }
            }

            Text {
                id: label
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 16 + entry.depth * 13
                anchors.rightMargin: 14

                text: entry.title.length > 0 ? entry.title : qsTr("(untitled)")
                elide: Text.ElideRight
                font.family: Theme.uiFont
                font.pixelSize: entry.depth === 0 ? 12.5 : 12
                font.weight: entry.current ? Font.DemiBold
                                           : (entry.depth === 0 ? Font.Medium : Font.Normal)
                color: entry.current ? Theme.accent
                                     : (entry.depth === 0 ? Theme.text : Theme.textMuted)
            }

            MouseArea {
                id: mouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.activated(entry.index)
            }
        }
    }

    Text {
        anchors.centerIn: parent
        width: parent.width - 32
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        visible: list.count === 0
        text: qsTr("No headings in this document")
        font.family: Theme.uiFont
        font.pixelSize: 12
        color: Theme.textFaint
    }
}
