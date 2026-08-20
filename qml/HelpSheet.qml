import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader

Popup {
    id: root

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 0

    readonly property int preferredWidth: 620
    width: Math.min(preferredWidth, parent ? parent.width - 60 : preferredWidth)
    height: Math.min(600, parent ? parent.height - 60 : 600)
    anchors.centerIn: parent

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: 1
        border.color: Theme.border
    }

    readonly property var entries: [
        { section: qsTr("What this is") },
        { text: qsTr("MDReader shows a markdown file two ways: a reading view that renders it, and an editor that shows the raw source. Split view puts both side by side and keeps their scrolling together.") },
        { text: qsTr("Every file opens in its own tab, and whatever is left open comes back on the next launch. The outline pane lists the document's headings — click one to jump to it.") },
        { text: qsTr("Files are watched while they are open: if something else changes one on disk, a bar offers to reload it. Nothing is ever saved without being asked.") },
        { text: qsTr("Unsaved changes are also kept on disk as you type. If the app is killed or the machine reboots, those tabs come back with the changes still in them, still marked unsaved. Closing a tab or quitting normally clears them.") },

        { section: qsTr("Files") },
        { action: qsTr("New document"),     keys: ["Ctrl", "N"] },
        { action: qsTr("New tab"),          keys: ["Ctrl", "T"] },
        { action: qsTr("Open"),             keys: ["Ctrl", "O"],
          note: qsTr("Several at once; each gets a tab. Dropping files on the window works too.") },
        { action: qsTr("Save"),             keys: ["Ctrl", "S"] },
        { action: qsTr("Save as"),          keys: ["Ctrl", "Shift", "S"] },
        { action: qsTr("Reload from disk"), keys: ["F5"],
          note: qsTr("Discards unsaved changes in that tab.") },
        { action: qsTr("Close tab"),        keys: ["Ctrl", "W"],
          note: qsTr("Closing the last one leaves a blank document rather than quitting.") },

        { section: qsTr("Tabs") },
        { action: qsTr("Next tab"),         keys: ["Ctrl", "Tab"] },
        { action: qsTr("Previous tab"),     keys: ["Ctrl", "Shift", "Tab"] },
        { action: qsTr("Go to tab 1 to 8"), keys: ["Ctrl", "1"] },
        { action: qsTr("Go to the last tab"), keys: ["Ctrl", "9"] },
        { action: qsTr("Close a tab"),      keys: [qsTr("Middle-click")] },

        { section: qsTr("View") },
        { action: qsTr("Reading view or edit source"), keys: ["Ctrl", "E"],
          note: qsTr("Unavailable in split view, where both panes are already up.") },
        { action: qsTr("Split editor and preview"), keys: ["Ctrl", "\\"] },
        { action: qsTr("Show or hide the outline"), keys: ["Ctrl", "B"] },
        { action: qsTr("Larger text"),      keys: ["Ctrl", "+"],
          note: qsTr("Sizes the reading view and the editor separately, whichever you are in.") },
        { action: qsTr("Smaller text"),     keys: ["Ctrl", "-"] },
        { action: qsTr("Reset text size"),  keys: ["Ctrl", "0"] },
        { action: qsTr("Full screen"),      keys: ["F11"] },
        { text: qsTr("The Fonts button in the toolbar picks the reading and editor typefaces independently. Fonts marked “bundled” ship with the app and work on any machine.") },

        { section: qsTr("Find") },
        { action: qsTr("Find"),             keys: ["Ctrl", "F"],
          note: qsTr("Searches whichever pane you are looking at.") },
        { action: qsTr("Find next"),        keys: ["F3"] },
        { action: qsTr("Find previous"),    keys: ["Shift", "F3"] },
        { action: qsTr("Replace"),          keys: ["Ctrl", "H"],
          note: qsTr("Editor only. Replace overwrites the current match, then moves to the next.") },
        { action: qsTr("Close find"),       keys: ["Esc"] },

        { section: qsTr("Editing the source") },
        { action: qsTr("Undo"),             keys: ["Ctrl", "Z"] },
        { action: qsTr("Redo"),             keys: ["Ctrl", "Y"] },
        { action: qsTr("Cut"),              keys: ["Ctrl", "X"] },
        { action: qsTr("Copy"),             keys: ["Ctrl", "C"] },
        { action: qsTr("Paste"),            keys: ["Ctrl", "V"] },
        { action: qsTr("Select all"),       keys: ["Ctrl", "A"] },
        { action: qsTr("Indent the selection"), keys: ["Tab"] },
        { action: qsTr("Outdent the selection"), keys: ["Shift", "Tab"] },
        { action: qsTr("Continue a list"),  keys: ["Enter"],
          note: qsTr("Repeats the bullet, number or task box. Press it on an empty item to end the list.") },

        { section: qsTr("Reading view") },
        { action: qsTr("Copy the selection"), keys: ["Ctrl", "C"],
          note: qsTr("Select with the mouse first; the pane has no caret.") },
        { action: qsTr("Select all"),       keys: ["Ctrl", "A"] },
        { action: qsTr("Scroll a page"),    keys: ["Page Up"] },
        { action: qsTr("Jump to the top or bottom"), keys: ["Home"] },
        { action: qsTr("Autoscroll"),       keys: [qsTr("Middle-drag")],
          note: qsTr("Hold the middle button and move up or down; how far you move sets the speed.") },

        { section: qsTr("Help") },
        { action: qsTr("Markdown reference"), keys: ["F1"] },
        { action: qsTr("This window"),      keys: ["Shift", "F1"] }
    ]

    contentItem: ColumnLayout {
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 46

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: Theme.border
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 18
                anchors.rightMargin: 8
                spacing: 8

                Text {
                    text: qsTr("MDReader help")
                    font.family: Theme.uiFont
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    color: Theme.textStrong
                }

                Text {
                    Layout.fillWidth: true
                    text: Qt.application.version
                    font.family: Theme.uiFont
                    font.pixelSize: 11
                    color: Theme.textFaint
                }

                IconButton {
                    glyph: Theme.iconClose
                    tip: qsTr("Close (Esc)")
                    onClicked: root.close()
                }
            }
        }

        ListView {
            id: list

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 6
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model: root.entries

            ScrollBar.vertical: SlimScrollBar {}

            delegate: Item {
                id: row

                required property var modelData

                readonly property bool isSection: row.modelData.section !== undefined
                readonly property bool isProse: row.modelData.text !== undefined
                readonly property bool isShortcut: row.modelData.keys !== undefined
                readonly property bool hasNote: row.modelData.note !== undefined

                readonly property int keyColumnWidth: 200

                width: list.width

                height: {
                    if (row.isSection)
                        return 36;
                    if (row.isProse)
                        return prose.implicitHeight + 14;
                    const body = 9 + actionLabel.implicitHeight
                               + (row.hasNote ? 2 + noteLabel.implicitHeight : 0) + 9;
                    return Math.max(35, body);
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 7
                    visible: row.isSection
                    text: row.isSection ? row.modelData.section : ""
                    font.family: Theme.uiFont
                    font.pixelSize: 10
                    font.letterSpacing: 1.1
                    font.weight: Font.DemiBold
                    color: Theme.textFaint
                }

                Text {
                    id: prose

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    anchors.top: parent.top
                    anchors.topMargin: 3
                    visible: row.isProse
                    text: row.isProse ? row.modelData.text : ""
                    wrapMode: Text.Wrap
                    lineHeight: 1.35
                    font.family: Theme.uiFont
                    font.pixelSize: 12
                    color: Theme.textMuted
                }

                Row {
                    id: keyRow

                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.top: parent.top
                    anchors.topMargin: 7
                    spacing: 4
                    visible: row.isShortcut

                    Repeater {
                        model: row.isShortcut ? row.modelData.keys : []

                        delegate: Rectangle {
                            id: cap

                            required property string modelData

                            width: capText.implicitWidth + 14
                            height: 21
                            radius: Theme.radiusSmall
                            color: Theme.background
                            border.width: 1
                            border.color: Theme.borderStrong

                            Text {
                                id: capText
                                anchors.centerIn: parent
                                text: cap.modelData
                                font.family: Theme.uiFont
                                font.pixelSize: 11
                                color: Theme.text
                            }
                        }
                    }
                }

                Text {
                    id: actionLabel

                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.right: parent.right
                    anchors.rightMargin: row.keyColumnWidth
                    anchors.top: parent.top
                    anchors.topMargin: 9
                    visible: row.isShortcut
                    text: row.isShortcut ? row.modelData.action : ""
                    wrapMode: Text.Wrap
                    font.family: Theme.uiFont
                    font.pixelSize: 12
                    color: Theme.text
                }

                Text {
                    id: noteLabel

                    anchors.left: actionLabel.left
                    anchors.right: actionLabel.right
                    anchors.top: actionLabel.bottom
                    anchors.topMargin: 2
                    visible: row.isShortcut && row.hasNote
                    text: row.isShortcut && row.hasNote ? row.modelData.note : ""
                    wrapMode: Text.Wrap
                    font.family: Theme.uiFont
                    font.pixelSize: 10
                    font.italic: true
                    color: Theme.textFaint
                }
            }
        }
    }
}
