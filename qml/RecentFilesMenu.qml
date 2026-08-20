import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader

Popup {
    id: root

    signal fileChosen(string path)

    implicitWidth: 380
    padding: 6
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    readonly property int rowHeight: 28
    readonly property int maxVisibleRows: 10

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: 1
        border.color: Theme.border
    }

    contentItem: ColumnLayout {
        spacing: 2

        ListView {
            id: list

            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(count, root.maxVisibleRows) * root.rowHeight
            visible: count > 0
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: AppSettings.recentFiles

            ScrollBar.vertical: SlimScrollBar {}

            delegate: Rectangle {
                id: entry

                required property int index
                required property string modelData

                width: list.width
                height: root.rowHeight
                radius: Theme.radiusSmall
                color: hover.containsMouse ? Theme.surfaceAlt : "transparent"

                Text {
                    id: entryGlyph
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    text: Theme.iconFolder
                    font.family: Theme.iconFont
                    font.pixelSize: 12
                    color: Theme.textFaint
                }

                Text {
                    anchors.left: entryGlyph.right
                    anchors.leftMargin: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    text: entry.modelData
                    elide: Text.ElideMiddle
                    font.family: Theme.uiFont
                    font.pixelSize: 12
                    color: hover.containsMouse ? Theme.textStrong : Theme.text
                }

                MouseArea {
                    id: hover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.close();
                        root.fileChosen(entry.modelData);
                    }
                }
            }
        }

        Text {
            Layout.fillWidth: true
            Layout.margins: 10
            visible: list.count === 0
            text: qsTr("No recent files")
            font.family: Theme.uiFont
            font.pixelSize: 12
            color: Theme.textFaint
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.topMargin: 2
            Layout.bottomMargin: 2
            visible: list.count > 0
            implicitHeight: 1
            color: Theme.border
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.rowHeight
            visible: list.count > 0
            radius: Theme.radiusSmall
            color: clearHover.containsMouse ? Theme.surfaceAlt : "transparent"

            Text {
                id: clearGlyph
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: Theme.iconClear
                font.family: Theme.iconFont
                font.pixelSize: 12
                color: Theme.textFaint
            }

            Text {
                anchors.left: clearGlyph.right
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Clear recent files")
                font.family: Theme.uiFont
                font.pixelSize: 12
                color: Theme.textMuted
            }

            MouseArea {
                id: clearHover
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    AppSettings.clearRecentFiles();
                    root.close();
                }
            }
        }
    }
}
