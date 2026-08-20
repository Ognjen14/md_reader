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

    readonly property int preferredWidth: 640
    width: Math.min(preferredWidth, parent ? parent.width - 60 : preferredWidth)
    height: Math.min(560, parent ? parent.height - 60 : 560)
    anchors.centerIn: parent

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: 1
        border.color: Theme.border
    }

    readonly property var entries: [
        { section: qsTr("Basic syntax") },
        { element: qsTr("Heading"),         syntax: "# H1\n## H2\n### H3" },
        { element: qsTr("Bold"),            syntax: "**bold text**" },
        { element: qsTr("Italic"),          syntax: "*italicized text*" },
        { element: qsTr("Blockquote"),      syntax: "> blockquote" },
        { element: qsTr("Ordered list"),    syntax: "1. First item\n2. Second item" },
        { element: qsTr("Unordered list"),  syntax: "- First item\n- Second item" },
        { element: qsTr("Code"),            syntax: "`code`" },
        { element: qsTr("Horizontal rule"), syntax: "---" },
        { element: qsTr("Link"),            syntax: "[title](https://example.com)" },
        { element: qsTr("Image"),           syntax: "![alt text](image.jpg)" },

        { section: qsTr("Extended syntax") },
        { element: qsTr("Table"),           syntax: "| Syntax | Description |\n| --- | --- |\n| Header | Title |" },
        { element: qsTr("Fenced code"),     syntax: "```json\n{ \"key\": \"value\" }\n```" },
        { element: qsTr("Strikethrough"),   syntax: "~~The world is flat.~~" },
        { element: qsTr("Task list"),       syntax: "- [x] Write the release\n- [ ] Update the site" },
        { element: qsTr("Footnote"),        syntax: "Some text. [^1]\n\n[^1]: The footnote." },
        { element: qsTr("Heading ID"),      syntax: "### My Heading {#custom-id}",
          note: qsTr("id is hidden in preview") },
        { element: qsTr("Highlight"),       syntax: "==very important words==" },
        { element: qsTr("Subscript"),       syntax: "H~2~O" },
        { element: qsTr("Superscript"),     syntax: "X^2^" },
        { element: qsTr("Emoji"),           syntax: "That is funny! :joy:" },
        { element: qsTr("Definition list"), syntax: "term\n: definition",
          note: qsTr("editor only") }
    ]

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: Theme.surfaceAlt
            radius: Theme.radius

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: parent.radius
                color: parent.color
            }

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
                    Layout.fillWidth: true
                    text: qsTr("Markdown reference")
                    font.family: Theme.uiFont
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    color: Theme.textStrong
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
                readonly property int labelWidth: 140

                width: list.width
                height: row.isSection ? 36 : syntaxBox.height + 10

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
                    id: elementLabel
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.top: parent.top
                    anchors.topMargin: 12
                    width: row.labelWidth - 20
                    visible: !row.isSection
                    text: row.isSection ? "" : row.modelData.element
                    wrapMode: Text.Wrap
                    font.family: Theme.uiFont
                    font.pixelSize: 12
                    color: Theme.text
                }

                Text {
                    anchors.left: elementLabel.left
                    anchors.top: elementLabel.bottom
                    anchors.topMargin: 2
                    width: elementLabel.width
                    visible: !row.isSection && row.modelData.note !== undefined
                    text: row.isSection || row.modelData.note === undefined
                          ? "" : row.modelData.note
                    wrapMode: Text.Wrap
                    font.family: Theme.uiFont
                    font.pixelSize: 10
                    font.italic: true
                    color: Theme.textFaint
                }

                Rectangle {
                    id: syntaxBox

                    anchors.left: parent.left
                    anchors.leftMargin: row.labelWidth
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.top: parent.top
                    anchors.topMargin: 5
                    height: syntaxText.implicitHeight + 16
                    visible: !row.isSection
                    radius: Theme.radiusSmall
                    color: Theme.mdInlineCodeBg

                    Text {
                        id: syntaxText
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 8
                        text: row.isSection ? "" : row.modelData.syntax
                        font.family: Theme.monoFont
                        font.pixelSize: 11
                        color: Theme.mdInlineCode
                        wrapMode: Text.NoWrap
                        elide: Text.ElideRight
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Theme.border
        }

        Text {
            Layout.fillWidth: true
            Layout.margins: 10
            text: qsTr("Press F1 to open this, Esc to close.")
            horizontalAlignment: Text.AlignHCenter
            font.family: Theme.uiFont
            font.pixelSize: 11
            color: Theme.textFaint
        }
    }
}
