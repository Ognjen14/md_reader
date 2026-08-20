import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader

Rectangle {
    id: root

    property bool editMode: false
    property bool splitView: false
    property bool syncScroll: true
    property bool outlineVisible: true
    property bool dirty: false
    property string documentName: ""
    property string documentPath: ""

    signal newRequested()
    signal openRequested()
    signal openPathRequested(string path)
    signal saveRequested()
    signal reloadRequested()
    signal findRequested()
    signal editModeToggled()
    signal splitViewToggled()
    signal syncScrollToggled()
    signal outlineToggled()
    signal themeToggled()
    signal helpRequested()
    signal appHelpRequested()

    signal dismissed()

    implicitHeight: Theme.toolbarHeight
    color: Theme.surface

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: Theme.border
    }

    RecentFilesMenu {
        id: recentMenu
        x: 34
        y: root.height - 2
        onFileChosen: (path) => root.openPathRequested(path)
        onClosed: root.dismissed()
    }

    FontPicker {
        id: fontPicker
        x: root.width - width - 6
        y: root.height - 2
        onClosed: root.dismissed()
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        spacing: 2

        IconButton {
            glyph: Theme.iconNew
            tip: qsTr("New (Ctrl+N)")
            onClicked: root.newRequested()
        }

        IconButton {
            glyph: Theme.iconOpen
            tip: qsTr("Open (Ctrl+O)")
            onClicked: root.openRequested()
        }

        IconButton {
            id: recentButton
            glyph: Theme.iconChevronDown
            tip: qsTr("Recent files")
            implicitWidth: 22
            checked: recentMenu.opened
            onClicked: recentMenu.opened ? recentMenu.close() : recentMenu.open()
        }

        IconButton {
            glyph: Theme.iconSave
            tip: qsTr("Save (Ctrl+S)")
            onClicked: root.saveRequested()
        }

        IconButton {
            glyph: Theme.iconReload
            tip: qsTr("Reload from disk (F5)")
            onClicked: root.reloadRequested()
        }

        Rectangle {
            Layout.leftMargin: 6
            Layout.rightMargin: 6
            Layout.preferredWidth: 1
            Layout.preferredHeight: 18
            color: Theme.border
        }

        IconButton {
            glyph: Theme.iconOutline
            tip: qsTr("Toggle outline (Ctrl+B)")
            checked: root.outlineVisible
            onClicked: root.outlineToggled()
        }

        IconButton {
            glyph: Theme.iconFind
            tip: qsTr("Find (Ctrl+F)")
            onClicked: root.findRequested()
        }

        Item { Layout.fillWidth: true; Layout.preferredHeight: 1 }

        Text {
            Layout.maximumWidth: 460
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideMiddle
            font.family: Theme.uiFont
            font.pixelSize: 12
            color: Theme.textMuted
            text: (root.dirty ? "• " : "") + (root.documentName.length > 0
                                                   ? root.documentName
                                                   : qsTr("Untitled"))

            ToolTip.visible: hoverArea.containsMouse && root.documentPath.length > 0
            ToolTip.delay: 600
            ToolTip.text: root.documentPath

            MouseArea {
                id: hoverArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.NoButton
            }
        }

        Item { Layout.fillWidth: true; Layout.preferredHeight: 1 }

        IconButton {
            glyph: root.editMode ? Theme.iconView : Theme.iconEdit
            enabled: !root.splitView
            tip: root.editMode ? qsTr("Reading view (Ctrl+E)") : qsTr("Edit source (Ctrl+E)")
            checked: root.editMode
            onClicked: root.editModeToggled()
        }

        IconButton {
            glyph: Theme.iconSplit
            tip: qsTr("Split editor and preview (Ctrl+\\)")
            checked: root.splitView
            onClicked: root.splitViewToggled()
        }

        IconButton {
            glyph: Theme.iconSync
            enabled: root.splitView
            tip: qsTr("Sync scrolling between editor and preview")
            checked: root.syncScroll
            onClicked: root.syncScrollToggled()
        }

        Rectangle {
            Layout.leftMargin: 6
            Layout.rightMargin: 6
            Layout.preferredWidth: 1
            Layout.preferredHeight: 18
            color: Theme.border
        }

        IconButton {
            glyph: Theme.iconTypography
            tip: qsTr("Fonts")
            checked: fontPicker.opened
            onClicked: fontPicker.opened ? fontPicker.close() : fontPicker.open()
        }

        IconButton {
            glyph: Theme.dark ? Theme.iconLight : Theme.iconDark
            tip: Theme.dark ? qsTr("Light theme") : qsTr("Dark theme")
            onClicked: root.themeToggled()
        }

        IconButton {
            glyph: Theme.iconHelp
            tip: qsTr("Markdown reference (F1)")
            onClicked: root.helpRequested()
        }

        IconButton {
            glyph: Theme.iconKeyboard
            tip: qsTr("Help and keyboard shortcuts (Shift+F1)")
            onClicked: root.appHelpRequested()
        }
    }
}
