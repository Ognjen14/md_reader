import QtQuick
import MDReader

Item {
    id: root

    property Flickable target: null

    readonly property int deadZone: 14
    readonly property real speedFactor: 0.28
    readonly property int maxStep: 60

    readonly property bool active: dragArea.pressed && root.target !== null

    property real anchorY: 0
    property real pointerY: 0

    anchors.fill: parent

    MouseArea {
        id: dragArea
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton
        cursorShape: root.active ? Qt.SizeVerCursor : Qt.ArrowCursor
        preventStealing: true

        onPressed: (mouse) => {
            root.anchorY = mouse.y;
            root.pointerY = mouse.y;
        }
        onPositionChanged: (mouse) => root.pointerY = mouse.y
    }

    Timer {
        running: root.active
        interval: 16
        repeat: true

        onTriggered: {
            const distance = root.pointerY - root.anchorY;
            if (Math.abs(distance) < root.deadZone)
                return;
            const beyond = distance - Math.sign(distance) * root.deadZone;
            const step = Math.max(-root.maxStep,
                                  Math.min(root.maxStep, beyond * root.speedFactor));
            const limit = Math.max(0, root.target.contentHeight - root.target.height);
            root.target.contentY = Math.max(0, Math.min(limit, root.target.contentY + step));
        }
    }

    Rectangle {
        x: root.width / 2 - width / 2
        y: root.anchorY - height / 2
        width: 26
        height: 26
        radius: width / 2
        visible: root.active
        color: Theme.surface
        border.width: 1
        border.color: Theme.borderStrong
        opacity: 0.9

        Text {
            anchors.centerIn: parent
            text: Theme.iconSort
            font.family: Theme.iconFont
            font.pixelSize: 12
            color: Theme.textMuted
        }
    }
}
