import QtQuick
import QtQuick.Controls.Basic
import MDReader

AbstractButton {
    id: control

    property string glyph: ""
    property string tip: ""
    property bool danger: false

    implicitWidth: 34
    implicitHeight: 30
    hoverEnabled: true
    focusPolicy: Qt.NoFocus

    background: Rectangle {
        radius: Theme.radiusSmall
        color: {
            if (!control.enabled)
                return "transparent";
            if (control.down)
                return Theme.overlay;
            if (control.checked)
                return Theme.accentSoft;
            return control.hovered ? Theme.surfaceAlt : "transparent";
        }

        Behavior on color {
            ColorAnimation { duration: Theme.animFast }
        }
    }

    contentItem: Text {
        text: control.glyph
        font.family: Theme.iconFont
        font.pixelSize: 15
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: {
            if (!control.enabled)
                return Theme.textFaint;
            if (control.danger)
                return Theme.danger;
            if (control.checked)
                return Theme.accent;
            return control.hovered ? Theme.textStrong : Theme.textMuted;
        }

        Behavior on color {
            ColorAnimation { duration: Theme.animFast }
        }
    }

    ToolTip.visible: control.hovered && control.tip.length > 0
    ToolTip.delay: 600
    ToolTip.text: control.tip
}
