import QtQuick
import MDReader

Item {
    id: root

    property int currentWidth: 0
    property int minimumWidth: 160
    property int maximumWidth: 520

    signal resized(int width)
    signal committed(int width)

    implicitWidth: 7

    Rectangle {
        anchors.centerIn: parent
        width: dragArea.pressed ? 2 : 1
        height: parent.height
        color: dragArea.pressed ? Theme.accent
                                : (dragArea.containsMouse ? Theme.borderStrong : Theme.border)

        Behavior on color {
            ColorAnimation { duration: Theme.animFast }
        }
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.SplitHCursor
        preventStealing: true

        property real pressSceneX: 0
        property real startWidth: 0

        onPressed: (mouse) => {
            pressSceneX = mapToItem(null, mouse.x, 0).x;
            startWidth = root.currentWidth;
        }

        onPositionChanged: (mouse) => {
            if (!pressed)
                return;
            const delta = mapToItem(null, mouse.x, 0).x - pressSceneX;
            root.resized(Math.round(Math.max(root.minimumWidth,
                                             Math.min(root.maximumWidth, startWidth + delta))));
        }

        onReleased: root.committed(root.currentWidth)
    }
}
