pragma Singleton

import QtQuick
import MDReader

QtObject {
    id: theme

    readonly property bool dark: AppSettings.darkTheme

    readonly property color background:  dark ? "#16171a" : "#fbfbfd"
    readonly property color surface:     dark ? "#1c1e22" : "#f3f4f8"
    readonly property color surfaceAlt:  dark ? "#23262c" : "#e9ebf1"
    readonly property color overlay:     dark ? "#2b2f36" : "#dfe2ea"
    readonly property color border:      dark ? "#2e323a" : "#dcdfe7"
    readonly property color borderStrong: dark ? "#3d424c" : "#c4c8d2"

    readonly property color text:        dark ? "#d9dce3" : "#22242a"
    readonly property color textStrong:  dark ? "#f0f2f6" : "#0d0e12"
    readonly property color textMuted:   dark ? "#8d93a0" : "#5d626d"
    readonly property color textFaint:   dark ? "#636874" : "#8b909b"

    readonly property color accent:      dark ? "#7aa2f7" : "#3057c8"
    readonly property color accentHover: dark ? "#93b5ff" : "#1f42a8"
    readonly property color accentSoft:  dark ? "#22314f" : "#dde5fb"
    readonly property color selection:   dark ? "#31456e" : "#bdd3f7"
    readonly property color warning:     dark ? "#e0af68" : "#8a5a00"
    readonly property color danger:      dark ? "#f7768e" : "#b3243c"




    readonly property color mdBackground: dark ? "#0f1014" : "#ffffff"
    readonly property color mdText:       dark ? "#e4e4e6" : "#1f2328"

    readonly property color mdHeading: mdText

    readonly property color mdBold: mdText
    readonly property color mdItalic: mdText
    readonly property color mdLink: dark ? "#6785e0" : "#0969da"

    readonly property color mdCode: mdText
    readonly property color mdInlineCode: dark ? "#6785e0" : "#0550ae"

    readonly property color mdMarker: dark ? "#7f96d8" : "#315d9e"
    readonly property color mdQuote: dark ? "#a7abb4" : "#57606a"
    readonly property color mdDelimiter: dark ? "#656b76" : "#8c959f"
    readonly property color mdRule: dark ? "#252a32" : "#d8dde3"

    readonly property color mdCodeBg: dark ? "#1d222a" : "#f6f8fa"
    readonly property color mdCodeKeyword:  dark ? "#c586c0" : "#af00db"
    readonly property color mdCodeType:     dark ? "#4ec9b0" : "#267f99"
    readonly property color mdCodeString:   dark ? "#ce9178" : "#a31515"
    readonly property color mdCodeNumber:   dark ? "#b5cea8" : "#098658"
    readonly property color mdCodeComment:  dark ? "#6a9955" : "#008000"
    readonly property color mdCodeFunction: dark ? "#dcdcaa" : "#795e26"
    readonly property color mdCodeVariable: dark ? "#9cdcfe" : "#001080"
    readonly property color mdInlineCodeBg: dark ? "#181c29" : "#eff2f6"
    readonly property color mdMarkBg: dark ? "#263a63" : "#dbe7ff"

    readonly property color mdChecked: dark ? "#8b949e" : "#57606a"
    readonly property color mdUnchecked: dark ? "#6e7681" : "#8c959f"
    readonly property color mdTableBorder: dark ? "#30363d" : "#d0d7de"
    readonly property color mdTableHead: dark ? "#1d222a" : "#f6f8fa"
    readonly property color mdTableStripe: dark ? "#1d222a" : "#f6f8fa"

    readonly property int radius: 6
    readonly property int radiusSmall: 4
    readonly property int toolbarHeight: 42
    readonly property int statusHeight: 26
    readonly property int readingWidth: 800
    readonly property int pagePadding: 36
    readonly property real readingLineHeight: 150

    readonly property int animFast: 110
    readonly property int animNormal: 180

    readonly property string uiFont: "Segoe UI"
    readonly property string monoFont: {
        const preferred = ["Cascadia Code", "Cascadia Mono", "JetBrains Mono", "Consolas"];
        const installed = Qt.fontFamilies();
        for (let i = 0; i < preferred.length; ++i) {
            if (installed.indexOf(preferred[i]) !== -1)
                return preferred[i];
        }
        return "monospace";
    }
    readonly property string iconFont: "Segoe MDL2 Assets"

    readonly property string readingFont: AppSettings.viewFontFamily.length > 0
        ? AppSettings.viewFontFamily : uiFont
    readonly property string editorFont: AppSettings.editorFontFamily.length > 0
        ? AppSettings.editorFontFamily : monoFont

    readonly property string iconChevronUp: String.fromCharCode(0xE70E)
    readonly property string iconFolder: String.fromCharCode(0xE8B7)
    readonly property string iconClear: String.fromCharCode(0xE74D)
    readonly property string iconSplit: String.fromCharCode(0xE8B4)
    readonly property string iconHelp: String.fromCharCode(0xE897)
    readonly property string iconSort: String.fromCharCode(0xE8CB)
    readonly property string iconTypography: String.fromCharCode(0xE8D2)
    readonly property string iconSync: String.fromCharCode(0xE895)
    readonly property string iconKeyboard: String.fromCharCode(0xE765)
    readonly property string iconCheck: String.fromCharCode(0xE73E)

    readonly property string iconCut: String.fromCharCode(0xE8C6)
    readonly property string iconCopy: String.fromCharCode(0xE8C8)
    readonly property string iconPaste: String.fromCharCode(0xE77F)
    readonly property string iconSelectAll: String.fromCharCode(0xE8B3)
    readonly property string iconUndo: String.fromCharCode(0xE7A7)
    readonly property string iconRedo: String.fromCharCode(0xE7A6)

    readonly property string glyphChecked: String.fromCharCode(0xE73A)
    readonly property string glyphUnchecked: String.fromCharCode(0xE739)

    readonly property string iconNew: ""
    readonly property string iconOpen: ""
    readonly property string iconSave: ""
    readonly property string iconReload: ""
    readonly property string iconEdit: ""
    readonly property string iconView: ""
    readonly property string iconOutline: ""
    readonly property string iconDark: ""
    readonly property string iconLight: ""
    readonly property string iconFind: ""
    readonly property string iconClose: ""
    readonly property string iconChevronDown: ""
    readonly property string iconWarning: ""
}
