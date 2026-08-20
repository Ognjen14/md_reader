import QtQuick
import QtQuick.Controls.Basic
import MDReader

Rectangle {
    id: root

    property var model: null
    property int currentIndex: 0

    signal selected(int index)
    signal closeRequested(int index)
    signal newTabRequested()

    readonly property int tabHeight: 34
    readonly property int maxTabWidth: 200
    readonly property int minTabWidth: 110

    implicitHeight: tabHeight
    color: Theme.surfaceAlt

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.border
    }

    ListView {
        id: list

        anchors.left: parent.left
        anchors.right: newTabButton.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        orientation: ListView.Horizontal
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        model: root.model
        currentIndex: root.currentIndex

        onCurrentIndexChanged: positionViewAtIndex(currentIndex, ListView.Contain)

        delegate: Item {
            id: tab

            required property int index
            required property string title
            required property string path
            required property bool dirty

            readonly property bool current: tab.index === root.currentIndex

            width: Math.max(root.minTabWidth,
                            Math.min(root.maxTabWidth, label.implicitWidth + 62))
            height: root.tabHeight

            Rectangle {
                anchors.fill: parent
                color: tab.current ? Theme.background
                                   : (hover.containsMouse ? Theme.overlay : "transparent")

                Behavior on color {
                    ColorAnimation { duration: Theme.animFast }
                }
            }

            Rectangle {
                anchors.top: parent.top
                width: parent.width
                height: 2
                visible: tab.current
                color: Theme.accent
            }

            Rectangle {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: 1
                height: parent.height - 12
                visible: !tab.current
                color: Theme.border
            }

            Text {
                id: label

                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.right: closeButton.left
                anchors.rightMargin: 4
                anchors.verticalCenter: parent.verticalCenter

                text: (tab.dirty ? "• " : "") + tab.title
                elide: Text.ElideMiddle
                font.family: Theme.uiFont
                font.pixelSize: 12
                color: tab.current ? Theme.textStrong : Theme.textMuted
            }

            IconButton {
                id: closeButton

                anchors.right: parent.right
                anchors.rightMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                implicitWidth: 20
                implicitHeight: 20

                glyph: Theme.iconClose
                opacity: tab.current || hover.containsMouse ? 1 : 0
                onClicked: root.closeRequested(tab.index)

                Behavior on opacity {
                    NumberAnimation { duration: Theme.animFast }
                }
            }

            MouseArea {
                id: hover
                anchors.fill: parent
                anchors.rightMargin: 26
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.MiddleButton

                ToolTip.visible: containsMouse && tab.path.length > 0
                ToolTip.delay: 800
                ToolTip.text: tab.path

                onClicked: (mouse) => {
                    if (mouse.button === Qt.MiddleButton)
                        root.closeRequested(tab.index);
                    else
                        root.selected(tab.index);
                }
            }
        }
    }

    IconButton {
        id: newTabButton

        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter

        glyph: Theme.iconNew
        tip: qsTr("New tab (Ctrl+T)")
        onClicked: root.newTabRequested()
    }
}
