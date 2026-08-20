import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import MDReader

ApplicationWindow {
    id: root

    property string startupFile: ""

    property bool editMode: false
    property bool splitView: false
    property bool syncScroll: AppSettings.syncScroll

    property int outlineWidth: AppSettings.outlineWidth
    property int splitRatio: AppSettings.splitRatio

    ListModel {
        id: tabs
    }

    property int currentIndex: 0

    readonly property Item currentDocument: docRepeater.count > 0
        && currentIndex >= 0 && currentIndex < docRepeater.count
        ? docRepeater.itemAt(currentIndex)
        : null

    readonly property string displayName: currentDocument ? currentDocument.displayName
                                                          : qsTr("Untitled")
    readonly property bool dirty: currentDocument ? currentDocument.dirty : false

    property bool forceClose: false
    property var pendingAction: null

    width: AppSettings.windowWidth
    height: AppSettings.windowHeight
    x: AppSettings.windowX
    y: AppSettings.windowY
    minimumWidth: 560
    minimumHeight: 380
    color: Theme.background
    title: (dirty ? "• " : "") + displayName + " — MDReader"

    property rect normalGeometry: Qt.rect(0, 0, 0, 0)
    property int preFullScreenVisibility: Window.Windowed

    property int restoreVisibility: Window.Windowed

    onVisibilityChanged: {
        if (visibility === Window.Windowed || visibility === Window.Maximized)
            restoreVisibility = visibility;
    }

    onXChanged: captureGeometry()
    onYChanged: captureGeometry()
    onWidthChanged: captureGeometry()
    onHeightChanged: captureGeometry()

    function captureGeometry() {
        if (visibility === Window.Windowed)
            normalGeometry = Qt.rect(x, y, width, height);
    }

    function persistGeometry() {
        if (normalGeometry.width <= 0) {
            AppSettings.saveGeometry(0, 0, 0, 0, true);
            return;
        }
        AppSettings.saveGeometry(normalGeometry.x, normalGeometry.y,
                                 normalGeometry.width, normalGeometry.height,
                                 visibility === Window.Maximized);
    }

    function persistSession() {
        const paths = [];
        for (let i = 0; i < tabs.count; ++i) {
            const path = tabs.get(i).path;
            if (path.length > 0)
                paths.push(path);
        }
        AppSettings.openFiles = paths;
        AppSettings.currentTab = currentIndex;
    }

    property bool _restoring: true

    Component.onCompleted: {
        editMode = AppSettings.editMode;
        splitView = AppSettings.splitView;
        if (splitView)
            editMode = true;
        _restoring = false;

        normalGeometry = Qt.rect(AppSettings.windowX, AppSettings.windowY,
                                 AppSettings.windowWidth, AppSettings.windowHeight);

        try {
            const drafts = DraftStore.pending();
            for (let d = 0; d < drafts.length; ++d)
                recoverDraft(drafts[d]);

            const restore = AppSettings.openFiles;
            for (let i = 0; i < restore.length; ++i)
                openInNewTab(restore[i]);
            if (restore.length > 0)
                currentIndex = Math.min(AppSettings.currentTab, tabs.count - 1);

            if (startupFile.length > 0)
                openInNewTab(startupFile);
            if (tabs.count === 0)
                appendTab();
        } finally {
            visibility = AppSettings.windowMaximized ? Window.Maximized
                                                     : Window.Windowed;
        }
    }

    onClosing: (close) => {
        if (!forceClose && anyTabDirty()) {
            close.accepted = false;
            confirmCloseAll();
            return;
        }
        for (let i = 0; i < docRepeater.count; ++i) {
            const doc = docRepeater.itemAt(i);
            if (doc)
                doc.discardDraft();
        }

        persistSession();
        persistGeometry();
    }

    onEditModeChanged: {
        if (_restoring)
            return;
        AppSettings.editMode = editMode;
        returnFocusToEditor();
        if (findBar.active)
            Qt.callLater(updateSearch);
    }

    onSyncScrollChanged: {
        if (_restoring)
            return;
        AppSettings.syncScroll = syncScroll;
    }

    onSplitViewChanged: {
        if (_restoring)
            return;
        AppSettings.splitView = splitView;
        if (splitView && !editMode)
            editMode = true;
        if (findBar.active)
            updateSearch();
    }

    onCurrentIndexChanged: {
        if (findBar.active)
            updateSearch();
        returnFocusToEditor();
    }

    function handleSecondInstance(path) {
        if (visibility === Window.Minimized)
            visibility = restoreVisibility;
        raise();
        requestActivate();

        if (path && path.length > 0)
            openInNewTab(path);
    }

    function appendTab() {
        tabs.append({ title: qsTr("Untitled"), path: "", dirty: false });
        currentIndex = tabs.count - 1;
        return currentIndex;
    }

    function newDocument() {
        appendTab();
        editMode = true;
        Qt.callLater(focusCurrentEditor);
    }

    function focusCurrentEditor() {
        if (currentDocument)
            currentDocument.focusEditor();
    }

    function returnFocusToEditor() {
        if (editMode)
            Qt.callLater(focusCurrentEditor);
    }

    function recoverDraft(draft) {
        if (draft.path.length > 0 && pathUtil.exists(draft.path))
            openInNewTab(draft.path);
        else
            appendTab();

        if (currentDocument)
            currentDocument.restoreDraft(draft.key, draft.text);
    }

    function indexOfPath(path) {
        for (let i = 0; i < tabs.count; ++i) {
            if (tabs.get(i).path === path)
                return i;
        }
        return -1;
    }

    function openInNewTab(path) {
        const existing = indexOfPath(path);
        if (existing !== -1) {
            currentIndex = existing;
            return;
        }

        const reusable = currentDocument && currentDocument.isPristine;
        if (!reusable)
            appendTab();

        if (currentDocument)
            currentDocument.loadPath(path);
    }

    function closeTab(index) {
        if (index < 0 || index >= tabs.count)
            return;

        const doc = docRepeater.itemAt(index);
        if (doc && doc.dirty) {
            currentIndex = index;
            pendingAction = function () { forceCloseTab(index); };
            unsavedDialog.open();
            return;
        }
        forceCloseTab(index);
    }

    function forceCloseTab(index) {
        const doc = docRepeater.itemAt(index);
        if (doc)
            doc.discardDraft();

        tabs.remove(index);
        if (tabs.count === 0) {
            appendTab();
            return;
        }
        currentIndex = Math.min(index, tabs.count - 1);
        persistSession();
    }

    function anyTabDirty() {
        for (let i = 0; i < docRepeater.count; ++i) {
            const doc = docRepeater.itemAt(i);
            if (doc && doc.dirty)
                return true;
        }
        return false;
    }

    function confirmCloseAll() {
        for (let i = 0; i < docRepeater.count; ++i) {
            const doc = docRepeater.itemAt(i);
            if (!doc || !doc.dirty)
                continue;
            currentIndex = i;
            pendingAction = function () { confirmCloseAll(); };
            unsavedDialog.open();
            return;
        }
        forceClose = true;
        close();
    }

    function guard(action) {
        if (!dirty) {
            action();
            return;
        }
        pendingAction = action;
        unsavedDialog.open();
    }

    function runPending() {
        const action = pendingAction;
        pendingAction = null;
        if (action)
            action();
    }

    function openDocument() {
        if (currentDocument && currentDocument.hasFile)
            openDialog.currentFolder = pathUtil.folderUrl(currentDocument.filePath);
        openDialog.open();
    }

    function saveDocument() {
        if (!currentDocument)
            return false;
        if (!currentDocument.hasFile) {
            saveAsDialog.open();
            return false;
        }
        return currentDocument.save();
    }

    function saveDocumentAs() {
        if (currentDocument && currentDocument.hasFile) {
            saveAsDialog.currentFolder = pathUtil.folderUrl(currentDocument.filePath);
            saveAsDialog.selectedFile = pathUtil.pathToUrl(currentDocument.filePath);
        }
        saveAsDialog.open();
    }

    function reloadDocument() {
        if (!currentDocument || !currentDocument.hasFile)
            return;
        if (currentDocument.dirty) {
            reloadDialog.open();
            return;
        }
        currentDocument.reload();
    }

    function adjustFont(delta) {
        if (editMode)
            AppSettings.editorFontSize = AppSettings.editorFontSize + delta;
        else
            AppSettings.viewFontSize = AppSettings.viewFontSize + delta;
    }

    function resetFont() {
        if (editMode)
            AppSettings.editorFontSize = 11;
        else
            AppSettings.viewFontSize = 15;
    }

    function toggleFullScreen() {
        if (visibility === Window.FullScreen) {
            visibility = preFullScreenVisibility;
        } else {
            preFullScreenVisibility = visibility;
            visibility = Window.FullScreen;
        }
    }

    function openFind() {
        findBar.active = true;
        findBar.focusField();
        updateSearch();
    }

    function openReplace() {
        findBar.active = true;
        findBar.focusReplaceField();
        updateSearch();
    }

    function closeFind() {
        findBar.active = false;
        matchPosition = -1;
        if (currentDocument)
            currentDocument.clearMatch();
        if (editMode)
            focusCurrentEditor();
    }

    function replaceCurrent() {
        if (!editMode || !currentDocument || findBar.term.length === 0)
            return;

        const target = currentDocument.searchTarget;
        const selected = target.selectedText;

        if (selected.length > 0 && selected.toLowerCase() === findBar.term.toLowerCase()) {
            const at = target.selectionStart;
            target.remove(at, at + selected.length);
            target.insert(at, findBar.replacement);
            target.cursorPosition = at + findBar.replacement.length;
            updateSearch();
        } else {
            findNext(false);
        }
    }

    function replaceEvery() {
        if (!editMode || !currentDocument || findBar.term.length === 0)
            return;

        const target = currentDocument.searchTarget;
        const needle = findBar.term.toLowerCase();
        const haystack = target.getText(0, target.length).toLowerCase();

        const positions = [];
        for (let at = haystack.indexOf(needle); at !== -1;
             at = haystack.indexOf(needle, at + needle.length)) {
            positions.push(at);
        }
        if (positions.length === 0)
            return;

        for (let i = positions.length - 1; i >= 0; --i) {
            target.remove(positions[i], positions[i] + needle.length);
            target.insert(positions[i], findBar.replacement);
        }

        updateSearch();
    }

    function searchText() {
        if (!currentDocument)
            return "";
        const target = currentDocument.searchTarget;
        return target.getText(0, target.length).toLowerCase();
    }

    property int matchPosition: -1

    function updateSearch() {
        const term = findBar.term;
        if (term.length === 0 || !currentDocument) {
            findBar.matchCount = 0;
            findBar.currentMatch = 0;
            matchPosition = -1;
            if (currentDocument)
                currentDocument.clearMatch();
            return;
        }

        const haystack = searchText();
        const needle = term.toLowerCase();

        let count = 0;
        for (let at = haystack.indexOf(needle); at !== -1;
             at = haystack.indexOf(needle, at + needle.length)) {
            ++count;
        }
        findBar.matchCount = count;

        findFrom(Math.max(0, matchPosition), false, true);
    }

    function findNext(backwards) {
        if (findBar.term.length === 0 || !currentDocument)
            return;
        const from = Math.max(0, matchPosition);
        findFrom(backwards ? from : from + findBar.term.length, backwards, false);
    }

    function findFrom(origin, backwards, inclusive) {
        const needle = findBar.term.toLowerCase();
        if (needle.length === 0 || !currentDocument)
            return;

        const haystack = searchText();
        let at;
        if (backwards) {
            at = haystack.lastIndexOf(needle, Math.max(0, origin - needle.length));
            if (at === -1)
                at = haystack.lastIndexOf(needle);
        } else {
            at = haystack.indexOf(needle, inclusive ? origin : Math.max(0, origin));
            if (at === -1)
                at = haystack.indexOf(needle);
        }

        if (at === -1) {
            findBar.currentMatch = 0;
            matchPosition = -1;
            currentDocument.clearMatch();
            return;
        }

        matchPosition = at;
        currentDocument.showMatch(at, needle.length);

        let ordinal = 0;
        for (let scan = haystack.indexOf(needle); scan !== -1 && scan <= at;
             scan = haystack.indexOf(needle, scan + needle.length)) {
            ++ordinal;
        }
        findBar.currentMatch = ordinal;
    }

    function isMarkdownUrl(url) {
        return /\.(md|markdown|txt)$/i.test(url.toString());
    }

    FileIO {
        id: pathUtil
    }

    header: ColumnLayout {
        spacing: 0

        AppToolbar {
            Layout.fillWidth: true

            editMode: root.editMode
            splitView: root.splitView
            syncScroll: root.syncScroll
            outlineVisible: AppSettings.outlineVisible
            dirty: root.dirty
            documentName: root.displayName
            documentPath: root.currentDocument ? root.currentDocument.filePath : ""

            onNewRequested: root.newDocument()
            onOpenRequested: root.openDocument()
            onOpenPathRequested: (path) => root.openInNewTab(path)
            onSaveRequested: root.saveDocument()
            onReloadRequested: root.reloadDocument()
            onFindRequested: root.openFind()
            onEditModeToggled: root.editMode = !root.editMode
            onSplitViewToggled: root.splitView = !root.splitView
            onSyncScrollToggled: root.syncScroll = !root.syncScroll
            onOutlineToggled: AppSettings.outlineVisible = !AppSettings.outlineVisible
            onThemeToggled: AppSettings.darkTheme = !AppSettings.darkTheme
            onHelpRequested: cheatSheet.open()
            onAppHelpRequested: helpSheet.open()
            onDismissed: root.returnFocusToEditor()
        }

        TabStrip {
            Layout.fillWidth: true

            model: tabs
            currentIndex: root.currentIndex

            onSelected: (index) => root.currentIndex = index
            onCloseRequested: (index) => root.closeTab(index)
            onNewTabRequested: root.newDocument()
        }
    }

    footer: StatusStrip {
        words: root.currentDocument ? root.currentDocument.wordCount : 0
        characters: root.currentDocument ? root.currentDocument.charCount : 0
        line: root.currentDocument ? root.currentDocument.cursorLine : 1
        column: root.currentDocument ? root.currentDocument.cursorColumn : 1
        showCursor: root.editMode
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        FindBar {
            id: findBar
            Layout.fillWidth: true
            canReplace: root.editMode
            onTermChanged: root.updateSearch()
            onFindNext: root.findNext(false)
            onFindPrevious: root.findNext(true)
            onReplaceOne: root.replaceCurrent()
            onReplaceAll: root.replaceEvery()
            onClosed: root.closeFind()
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: root.currentIndex

            Repeater {
                id: docRepeater
                model: tabs

                DocumentView {
                    required property int index

                    editMode: root.editMode
                    splitView: root.splitView
                    outlineVisible: AppSettings.outlineVisible
                    outlineWidth: root.outlineWidth
                    splitRatio: root.splitRatio
                    syncScroll: root.syncScroll

                    onOutlineResized: (width) => {
                        root.outlineWidth = width;
                        AppSettings.outlineWidth = width;
                    }
                    onSplitResized: (ratio) => {
                        root.splitRatio = ratio;
                        AppSettings.splitRatio = ratio;
                    }

                    onErrorRaised: (message) => {
                        root.pendingAction = null;
                        errorDialog.message = message;
                        errorDialog.open();
                    }

                    onReloadRequested: {
                        root.currentIndex = index;
                        Qt.callLater(root.reloadDocument);
                    }

                    onDirtyChanged: tabs.setProperty(index, "dirty", dirty)
                    onDisplayNameChanged: tabs.setProperty(index, "title", displayName)
                    onFilePathChanged: {
                        tabs.setProperty(index, "path", filePath);
                        if (filePath.length > 0) {
                            AppSettings.addRecentFile(filePath);
                            root.persistSession();
                        }
                    }
                }
            }
        }
    }

    DropArea {
        anchors.fill: parent
        z: 50

        onEntered: (drag) => {
            drag.accepted = drag.hasUrls && drag.urls.some(root.isMarkdownUrl);
            dropOverlay.visible = drag.accepted;
        }
        onExited: dropOverlay.visible = false
        onDropped: (drop) => {
            dropOverlay.visible = false;
            if (!drop.hasUrls)
                return;
            drop.acceptProposedAction();
            const urls = drop.urls.filter(root.isMarkdownUrl);
            for (let i = 0; i < urls.length; ++i)
                root.openInNewTab(pathUtil.urlToPath(urls[i]));
        }
    }

    Rectangle {
        id: dropOverlay

        anchors.fill: parent
        z: 49
        visible: false
        color: Theme.dark ? "#cc16171a" : "#ccfbfbfd"

        Rectangle {
            anchors.centerIn: parent
            width: Math.min(340, parent.width - 60)
            height: 110
            radius: Theme.radius
            color: "transparent"
            border.width: 2
            border.color: Theme.accent

            Column {
                anchors.centerIn: parent
                spacing: 8

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Theme.iconOpen
                    font.family: Theme.iconFont
                    font.pixelSize: 26
                    color: Theme.accent
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Drop to open")
                    font.family: Theme.uiFont
                    font.pixelSize: 13
                    color: Theme.text
                }
            }
        }
    }

    Shortcut { sequences: [StandardKey.Open]; onActivated: root.openDocument() }
    Shortcut { sequences: [StandardKey.Save]; onActivated: root.saveDocument() }
    Shortcut { sequences: [StandardKey.SaveAs]; onActivated: root.saveDocumentAs() }
    Shortcut { sequences: [StandardKey.New]; onActivated: root.newDocument() }
    Shortcut { sequences: [StandardKey.AddTab]; onActivated: root.newDocument() }
    Shortcut { sequences: [StandardKey.Close]; onActivated: root.closeTab(root.currentIndex) }
    Shortcut { sequences: [StandardKey.NextChild]; onActivated: root.cycleTab(1) }
    Shortcut { sequences: [StandardKey.PreviousChild]; onActivated: root.cycleTab(-1) }
    Shortcut { sequence: "F5"; onActivated: root.reloadDocument() }
    Shortcut {
        sequence: "Ctrl+E"
        enabled: !root.splitView
        onActivated: root.editMode = !root.editMode
    }
    Shortcut { sequence: "Ctrl+\\"; onActivated: root.splitView = !root.splitView }
    Shortcut { sequence: "F1"; onActivated: cheatSheet.open() }
    Shortcut { sequence: "Shift+F1"; onActivated: helpSheet.open() }
    Shortcut {
        sequence: "Ctrl+B"
        onActivated: AppSettings.outlineVisible = !AppSettings.outlineVisible
    }
    Shortcut { sequences: ["Ctrl++", "Ctrl+="]; onActivated: root.adjustFont(1) }
    Shortcut { sequences: ["Ctrl+-"]; onActivated: root.adjustFont(-1) }
    Shortcut { sequence: "Ctrl+0"; onActivated: root.resetFont() }
    Shortcut { sequence: "F11"; onActivated: root.toggleFullScreen() }
    Shortcut { sequences: [StandardKey.Find]; onActivated: root.openFind() }
    Shortcut {
        sequences: [StandardKey.Replace]
        enabled: root.editMode
        onActivated: root.openReplace()
    }
    Shortcut { sequences: [StandardKey.FindNext]; onActivated: root.findNext(false) }
    Shortcut { sequences: [StandardKey.FindPrevious]; onActivated: root.findNext(true) }
    Shortcut {
        sequence: "Esc"
        enabled: findBar.active
        onActivated: root.closeFind()
    }

    function cycleTab(delta) {
        if (tabs.count < 2)
            return;
        currentIndex = (currentIndex + delta + tabs.count) % tabs.count;
    }

    Instantiator {
        model: 9

        delegate: Shortcut {
            required property int index
            sequence: "Ctrl+" + (index + 1)
            onActivated: root.currentIndex = index === 8
                ? tabs.count - 1
                : Math.min(index, tabs.count - 1)
        }
    }

    FileDialog {
        id: openDialog
        title: qsTr("Open markdown file")
        fileMode: FileDialog.OpenFiles
        nameFilters: [qsTr("Markdown (*.md *.markdown *.txt)"), qsTr("All files (*)")]

        onAccepted: {
            for (let i = 0; i < selectedFiles.length; ++i)
                root.openInNewTab(pathUtil.urlToPath(selectedFiles[i]));
            root.returnFocusToEditor();
        }
        onRejected: root.returnFocusToEditor()
    }

    FileDialog {
        id: saveAsDialog
        title: qsTr("Save markdown file")
        fileMode: FileDialog.SaveFile
        defaultSuffix: "md"
        nameFilters: [qsTr("Markdown (*.md *.markdown *.txt)"), qsTr("All files (*)")]

        onAccepted: {
            if (root.currentDocument && root.currentDocument.saveAs(selectedFile))
                root.runPending();
            else
                root.pendingAction = null;
            root.returnFocusToEditor();
        }
        onRejected: {
            root.pendingAction = null;
            root.returnFocusToEditor();
        }
    }

    AppDialog {
        id: unsavedDialog
        title: qsTr("Unsaved changes")
        message: qsTr("“%1” has changes that have not been saved.").arg(root.displayName)
        primaryText: qsTr("Save")
        secondaryText: qsTr("Discard")
        secondaryDestructive: true
        cancelText: qsTr("Cancel")

        onPrimaryClicked: {
            if (root.saveDocument())
                root.runPending();
        }
        onSecondaryClicked: {
            if (root.currentDocument) {
                root.currentDocument.dirty = false;
                root.currentDocument.discardDraft();
            }
            root.runPending();
        }
        onCancelClicked: root.pendingAction = null
        onClosed: root.returnFocusToEditor()
    }

    AppDialog {
        id: reloadDialog
        title: qsTr("Reload from disk")
        message: qsTr("Discard your changes and reload from disk?")
        detail: root.currentDocument ? root.currentDocument.filePath : ""
        primaryText: qsTr("Reload")
        cancelText: qsTr("Cancel")

        onPrimaryClicked: {
            if (root.currentDocument)
                root.currentDocument.reload();
        }
        onClosed: root.returnFocusToEditor()
    }

    AppDialog {
        id: errorDialog
        title: qsTr("Something went wrong")
        primaryText: qsTr("OK")
        onClosed: root.returnFocusToEditor()
    }

    CheatSheet {
        id: cheatSheet
        onClosed: root.returnFocusToEditor()
    }

    HelpSheet {
        id: helpSheet
        onClosed: root.returnFocusToEditor()
    }
}
