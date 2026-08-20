import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader


Item {
    id: root

    property bool editMode: false
    property bool splitView: false
    property bool outlineVisible: true
    property int outlineWidth: 260
    property int splitRatio: 50
    property bool syncScroll: true

    readonly property bool previewVisible: !editMode || splitView

    property string sourceText: ""
    property bool dirty: false

    readonly property string filePath: fileIO.filePath
    readonly property bool hasFile: fileIO.hasFile
    readonly property string displayName: fileIO.hasFile ? fileIO.fileName : qsTr("Untitled")
    readonly property bool isPristine: !dirty && !fileIO.hasFile && sourceText.length === 0

    property int wordCount: 0
    property int charCount: 0
    readonly property int cursorLine: editPane.cursorLine
    readonly property int cursorColumn: editPane.cursorColumn

    property bool _syncing: false

    function _syncScrollFrom(pane) {
        if (!root.syncScroll || root._syncing)
            return;
        if (!editPane.visible || !viewPane.visible)
            return;

        if (pane === viewPane && viewPane.rebuilding)
            return;

        root._syncing = true;
        if (pane === editPane)
            viewPane.setScrollFraction(editPane.scrollFraction);
        else
            editPane.setScrollFraction(viewPane.scrollFraction);
        root._syncing = false;
    }

    readonly property Item searchTarget: editMode ? editPane.textArea : viewPane.textItem

    property bool loading: false

    signal errorRaised(string message)
    signal outlineResized(int width)
    signal splitResized(int ratio)
    signal externalChangeDetected()
    signal reloadRequested()

    function loadPath(path) { return fileIO.loadPath(path); }
    function load(url) { return fileIO.load(url); }

    function save() {
        if (!fileIO.hasFile)
            return false;
        if (!fileIO.save(sourceText))
            return false;
        dirty = false;
        discardDraft();
        return true;
    }

    function saveAs(url) {
        if (!fileIO.saveAs(url, sourceText))
            return false;
        dirty = false;
        discardDraft();
        return true;
    }

    function clear() {
        fileIO.reset();
        setSource("");
        dirty = false;
        discardDraft();
        banner.active = false;
    }

    function reload() {
        if (!fileIO.hasFile)
            return;
        const fraction = viewPane.scrollFraction;
        const line = editPane.lineAt(editPane.textArea.cursorPosition);

        if (!fileIO.reload())
            return;

        banner.active = false;
        editPane.textArea.cursorPosition = editPane.positionForLine(line);
        Qt.callLater(viewPane.setScrollFraction, fraction);
    }

    function setSource(text) {
        loading = true;
        sourceText = text;
        loading = false;
        refreshPreview();
    }

    function focusEditor() { editPane.focusEditor(); }

    function showMatch(position, length) {
        if (editMode) {
            editPane.textArea.select(position, position + length);
            viewPane.clearMatch();
        } else {
            viewPane.highlightMatch(position, length);
            viewPane.ensureVisible(position);
        }
    }

    function clearMatch() {
        viewPane.clearMatch();
    }

    function revealMatch(position) {
        if (previewVisible && !editMode)
            viewPane.ensureVisible(position);
    }

    function dismissBanner() { banner.active = false; }

    property string draftKey: ""

    Timer {
        id: draftTimer
        interval: 2000
        onTriggered: root._writeDraft()
    }

    function _writeDraft() {
        if (!dirty)
            return;
        if (draftKey.length === 0)
            draftKey = DraftStore.newKey();
        DraftStore.write(draftKey, filePath, sourceText);
    }

    function discardDraft() {
        draftTimer.stop();
        if (draftKey.length === 0)
            return;
        DraftStore.discard(draftKey);
        draftKey = "";
    }

    function restoreDraft(key, text) {
        draftKey = key;
        setSource(text);
        dirty = true;
    }

    onSourceTextChanged: {
        if (!loading)
            dirty = true;
        if (dirty)
            draftTimer.restart();
        statsTimer.restart();
        outlineTimer.restart();
        if (previewVisible)
            previewTimer.restart();
    }

    onPreviewVisibleChanged: {
        if (previewVisible)
            refreshPreview();
    }

    property string previewSource: ""

    function refreshPreview() {
        previewTimer.stop();
        previewSource = sourceText;
    }

    Timer {
        id: previewTimer
        interval: Math.max(400, Math.min(1500, viewPane.lastRebuildMs * 12))
        onTriggered: root.previewSource = root.sourceText
    }

    Timer {
        id: outlineTimer
        interval: 350
        onTriggered: outlineModel.source = root.sourceText
    }

    Timer {
        id: statsTimer
        interval: 250
        onTriggered: {
            const text = root.sourceText;
            root.charCount = text.length;
            const trimmed = text.trim();
            root.wordCount = trimmed.length === 0 ? 0 : trimmed.split(/\s+/).length;
        }
    }

    DocumentOutline {
        id: outlineModel
    }

    FileIO {
        id: fileIO

        onLoaded: (text) => {
            root.setSource(text);
            root.dirty = false;
            root.discardDraft();
            banner.active = false;
        }

        onError: (message) => root.errorRaised(message)

        onFileChangedOnDisk: {
            if (root.dirty) {
                banner.active = true;
                root.externalChangeDetected();
            } else {
                root.reload();
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Banner {
            id: banner
            Layout.fillWidth: true
            message: qsTr("This file changed on disk.")
            actionText: qsTr("Reload")

            onActionTriggered: root.reloadRequested()
            onDismissed: active = false
        }

        RowLayout {
            id: paneRow

            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            readonly property int splitSpace: Math.max(0, width
                - (root.outlineVisible ? root.outlineWidth + outlineSplitter.implicitWidth : 0)
                - (root.splitView ? paneSplitter.implicitWidth : 0))

            OutlinePane {
                id: outlinePane

                Layout.preferredWidth: root.outlineWidth
                Layout.fillHeight: true
                visible: root.outlineVisible

                model: outlineModel
                currentIndex: root.previewVisible
                    ? viewPane.currentHeading
                    : outlineModel.indexForLine(editPane.cursorLine - 1)

                onActivated: (index) => {
                    if (root.previewVisible)
                        viewPane.scrollToHeading(index);
                    if (root.editMode)
                        editPane.goToLine(outlineModel.lineAt(index) + 1);
                }
            }

            SplitterHandle {
                id: outlineSplitter

                Layout.fillHeight: true
                visible: outlinePane.visible
                currentWidth: root.outlineWidth
                minimumWidth: 160
                maximumWidth: Math.min(520, root.width * 0.4)

                onResized: (width) => root.outlineResized(width)
                onCommitted: (width) => root.outlineResized(width)
            }

            EditMode {
                id: editPane

                Layout.fillHeight: true
                Layout.fillWidth: !root.splitView
                Layout.preferredWidth: root.splitView
                    ? Math.round(paneRow.splitSpace * root.splitRatio / 100)
                    : 0
                visible: root.editMode

                source: root.sourceText
                onSourceEdited: (text) => {
                    if (root.sourceText !== text)
                        root.sourceText = text;
                }
                onViewportYChanged: root._syncScrollFrom(editPane)
            }

            SplitterHandle {
                id: paneSplitter

                Layout.fillHeight: true
                visible: root.splitView
                currentWidth: editPane.width
                minimumWidth: Math.round(paneRow.splitSpace * 0.2)
                maximumWidth: Math.round(paneRow.splitSpace * 0.8)

                onResized: (width) => {
                    if (paneRow.splitSpace > 0)
                        root.splitResized(Math.round(width * 100 / paneRow.splitSpace));
                }
                onCommitted: (width) => {
                    if (paneRow.splitSpace > 0)
                        root.splitResized(Math.round(width * 100 / paneRow.splitSpace));
                }
            }

            ViewMode {
                id: viewPane

                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: root.previewVisible

                claimFocus: !root.editMode

                source: root.previewSource
                documentUrl: fileIO.pathToUrl(root.filePath)
                onViewportYChanged: root._syncScrollFrom(viewPane)
            }
        }
    }
}
