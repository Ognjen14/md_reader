import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader

Popup {
    id: root

    property bool editingEditor: false

    implicitWidth: 340
    padding: 8
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    readonly property int rowHeight: 30

    readonly property string current: root.editingEditor
        ? AppSettings.editorFontFamily : AppSettings.viewFontFamily

    readonly property var pool: root.editingEditor
        ? FontCatalog.monoFamilies : FontCatalog.families

    readonly property string fallback: root.editingEditor ? Theme.monoFont : Theme.uiFont

    readonly property var entries: {
        const needle = search.text.trim().toLowerCase();
        const source = root.pool;
        const bundled = [];
        const rest = [];

        for (let i = 0; i < source.length; ++i) {
            const family = source[i];
            if (needle.length > 0 && family.toLowerCase().indexOf(needle) === -1)
                continue;
            if (FontCatalog.bundled.indexOf(family) !== -1)
                bundled.push(family);
            else
                rest.push(family);
        }
        return bundled.concat(rest);
    }

    function choose(family) {
        if (root.editingEditor)
            AppSettings.editorFontFamily = family;
        else
            AppSettings.viewFontFamily = family;
    }

    onOpened: {
        search.text = "";
        search.forceActiveFocus();
        list.positionViewAtIndex(Math.max(0, root.entries.indexOf(root.current)),
                                 ListView.Center);
    }

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: 1
        border.color: Theme.border
    }

    contentItem: ColumnLayout {
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 4

            Repeater {
                model: [qsTr("Reading"), qsTr("Editor")]

                delegate: Rectangle {
                    id: tab

                    required property int index
                    required property string modelData

                    readonly property bool active: (tab.index === 1) === root.editingEditor

                    Layout.fillWidth: true
                    Layout.preferredHeight: 26
                    radius: Theme.radiusSmall
                    color: tab.active ? Theme.accentSoft
                                      : (tabHover.containsMouse ? Theme.surfaceAlt : "transparent")
                    border.width: 1
                    border.color: tab.active ? Theme.accent : Theme.border

                    Text {
                        anchors.centerIn: parent
                        text: tab.modelData
                        font.family: Theme.uiFont
                        font.pixelSize: 12
                        color: tab.active ? Theme.textStrong : Theme.textMuted
                    }

                    MouseArea {
                        id: tabHover
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.editingEditor = (tab.index === 1)
                    }
                }
            }
        }

        TextField {
            id: search

            Layout.fillWidth: true
            Layout.preferredHeight: 28

            placeholderText: qsTr("Search fonts")
            font.family: Theme.uiFont
            font.pixelSize: 12
            color: Theme.text
            placeholderTextColor: Theme.textFaint
            selectionColor: Theme.selection
            selectedTextColor: Theme.textStrong
            leftPadding: 8
            rightPadding: 8

            background: Rectangle {
                radius: Theme.radiusSmall
                color: Theme.background
                border.width: 1
                border.color: search.activeFocus ? Theme.accent : Theme.border
            }

            Keys.onDownPressed: list.forceActiveFocus()
        }

        ListView {
            id: list

            Layout.fillWidth: true
            Layout.preferredHeight: 300

            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: root.entries

            ScrollBar.vertical: SlimScrollBar {}

            delegate: Rectangle {
                id: entry

                required property int index
                required property string modelData

                readonly property bool chosen: entry.modelData === root.current
                readonly property bool isBundled: FontCatalog.bundled.indexOf(entry.modelData) !== -1

                width: list.width
                height: root.rowHeight
                radius: Theme.radiusSmall
                color: entryHover.containsMouse ? Theme.surfaceAlt
                                                : (entry.chosen ? Theme.accentSoft : "transparent")

                Text {
                    id: tick

                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter

                    text: Theme.iconCheck
                    font.family: Theme.iconFont
                    font.pixelSize: 11
                    color: Theme.accent
                    visible: entry.chosen
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 26
                    anchors.right: badge.left
                    anchors.rightMargin: 6
                    anchors.verticalCenter: parent.verticalCenter

                    text: entry.modelData
                    font.family: entry.modelData
                    font.pixelSize: 14
                    elide: Text.ElideRight
                    color: entry.chosen ? Theme.textStrong : Theme.text
                }

                Text {
                    id: badge

                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("bundled")
                    visible: entry.isBundled
                    font.family: Theme.uiFont
                    font.pixelSize: 10
                    color: Theme.textFaint
                }

                MouseArea {
                    id: entryHover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.choose(entry.modelData)
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 1
            color: Theme.border
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 26
            radius: Theme.radiusSmall
            color: resetHover.containsMouse ? Theme.surfaceAlt : "transparent"

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Use the default (%1)").arg(root.fallback)
                font.family: Theme.uiFont
                font.pixelSize: 11
                color: root.current.length === 0 ? Theme.textFaint : Theme.textMuted
            }

            MouseArea {
                id: resetHover
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.choose("")
            }
        }
    }
}
