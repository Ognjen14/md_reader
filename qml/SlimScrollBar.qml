import QtQuick
import QtQuick.Controls.Basic
import MDReader

ScrollBar {
    id: control

    policy: ScrollBar.AsNeeded
    minimumSize: 0.06

    contentItem: Rectangle {
        implicitWidth: control.pressed || control.hovered ? 8 : 5
        implicitHeight: control.pressed || control.hovered ? 8 : 5
        radius: width / 2
        color: control.pressed ? Theme.borderStrong : Theme.border
        opacity: control.policy === ScrollBar.AlwaysOn
                 || control.active
                 || control.hovered ? 1.0 : 0.0

        Behavior on implicitWidth {
            NumberAnimation { duration: Theme.animFast }
        }
        Behavior on implicitHeight {
            NumberAnimation { duration: Theme.animFast }
        }
        Behavior on opacity {
            NumberAnimation { duration: Theme.animNormal }
        }
    }

    background: Item {}
}
