import QtQuick
import QtQuick.Controls.Basic
import MDReader

Popup {
    id: root

    property var target: null

    implicitWidth: 230
    padding: 4
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    readonly property int rowHeight: 28
    readonly property int separatorHeight: 9

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: 1
        border.color: Theme.border
    }

    function popupAt(item, pos) {
        rebuild();
        const p = item.mapToItem(root.parent, pos.x, pos.y);
        root.x = Math.max(0, Math.min(p.x, root.parent.width - root.implicitWidth));
        root.y = Math.max(0, Math.min(p.y, root.parent.height - root.implicitHeight));
        root.open();
    }

    property var actions: []

    function rebuild() {
        const t = root.target;
        if (!t) {
            root.actions = [];
            return;
        }

        const editable = !t.readOnly;
        const selected = t.selectedText.length > 0;
        const list = [];

        if (editable) {
            list.push({ act: "undo", label: qsTr("Undo"), glyph: Theme.iconUndo,
                        hint: "Ctrl+Z", on: t.canUndo });
            list.push({ act: "redo", label: qsTr("Redo"), glyph: Theme.iconRedo,
                        hint: "Ctrl+Y", on: t.canRedo });
            list.push({ separator: true });
            list.push({ act: "cut", label: qsTr("Cut"), glyph: Theme.iconCut,
                        hint: "Ctrl+X", on: selected });
        }

        list.push({ act: "copy", label: qsTr("Copy"), glyph: Theme.iconCopy,
                    hint: "Ctrl+C", on: selected });

        if (editable) {
            list.push({ act: "paste", label: qsTr("Paste"), glyph: Theme.iconPaste,
                        hint: "Ctrl+V", on: t.canPaste });
        }

        list.push({ separator: true });
        list.push({ act: "selectAll", label: qsTr("Select all"), glyph: Theme.iconSelectAll,
                    hint: "Ctrl+A", on: t.length > 0 });

        root.actions = list;
    }

    function invoke(act) {
        const t = root.target;
        root.close();
        if (!t)
            return;

        switch (act) {
        case "undo":      t.undo(); break;
        case "redo":      t.redo(); break;
        case "cut":       t.cut(); break;
        case "copy":      t.copy(); break;
        case "paste":     t.paste(); break;
        case "selectAll": t.selectAll(); break;
        }
    }

    contentItem: Column {
        spacing: 0

        Repeater {
            model: root.actions

            delegate: Item {
                id: row

                required property var modelData

                readonly property bool isSeparator: row.modelData.separator === true
                readonly property bool usable: row.modelData.on === true

                width: root.availableWidth
                height: row.isSeparator ? root.separatorHeight : root.rowHeight

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width
                    height: 1
                    visible: row.isSeparator
                    color: Theme.border
                }

                Rectangle {
                    anchors.fill: parent
                    visible: !row.isSeparator
                    radius: Theme.radiusSmall
                    color: hover.containsMouse ? Theme.surfaceAlt : "transparent"

                    Text {
                        id: glyph

                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        text: row.modelData.glyph || ""
                        font.family: Theme.iconFont
                        font.pixelSize: 12
                        color: Theme.textFaint
                        opacity: row.usable ? 1 : 0.45
                    }

                    Text {
                        anchors.left: glyph.right
                        anchors.leftMargin: 8
                        anchors.right: hint.left
                        anchors.rightMargin: 12
                        anchors.verticalCenter: parent.verticalCenter

                        text: row.modelData.label || ""
                        elide: Text.ElideRight
                        font.family: Theme.uiFont
                        font.pixelSize: 12
                        color: !row.usable ? Theme.textFaint
                                           : (hover.containsMouse ? Theme.textStrong : Theme.text)
                    }

                    Text {
                        id: hint

                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        text: row.modelData.hint || ""
                        font.family: Theme.uiFont
                        font.pixelSize: 11
                        color: Theme.textFaint
                        opacity: row.usable ? 1 : 0.45
                    }

                    MouseArea {
                        id: hover

                        anchors.fill: parent
                        hoverEnabled: true
                        enabled: row.usable
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.invoke(row.modelData.act)
                    }
                }
            }
        }
    }
}
