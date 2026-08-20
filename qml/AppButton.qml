import QtQuick
import QtQuick.Controls.Basic
import MDReader

Button {
    id: control

    property bool primary: false
    property bool destructive: false

    implicitWidth: Math.max(88, contentLabel.implicitWidth + 28)
    implicitHeight: 32
    hoverEnabled: true

    background: Rectangle {
        radius: Theme.radius
        border.width: control.primary ? 0 : 1
        border.color: control.hovered ? Theme.borderStrong : Theme.border
        color: {
            if (control.primary)
                return control.down ? Theme.accentHover
                                    : (control.hovered ? Theme.accentHover : Theme.accent);
            if (control.down)
                return Theme.overlay;
            return control.hovered ? Theme.surfaceAlt : "transparent";
        }

        Behavior on color {
            ColorAnimation { duration: Theme.animFast }
        }
    }

    contentItem: Text {
        id: contentLabel
        text: control.text
        font.family: Theme.uiFont
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: {
            if (control.primary)
                return Theme.dark ? "#0d1017" : "#ffffff";
            if (control.destructive)
                return Theme.danger;
            return Theme.text;
        }
    }
}
