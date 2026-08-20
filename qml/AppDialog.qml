import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import MDReader

Dialog {
    id: control

    property string message: ""
    property string detail: ""
    property string primaryText: qsTr("OK")
    property string secondaryText: ""
    property string cancelText: ""
    property bool secondaryDestructive: false

    signal primaryClicked()
    signal secondaryClicked()
    signal cancelClicked()

    modal: true
    anchors.centerIn: parent
    closePolicy: Popup.CloseOnEscape
    padding: 20
    implicitWidth: 420

    onRejected: control.cancelClicked()

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: 1
        border.color: Theme.border
    }

    header: Item {
        visible: control.title.length > 0
        implicitHeight: control.title.length > 0 ? 44 : 0

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: control.title
            font.family: Theme.uiFont
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: Theme.textStrong
        }
    }

    contentItem: ColumnLayout {
        spacing: 6

        Text {
            Layout.fillWidth: true
            visible: control.message.length > 0
            text: control.message
            wrapMode: Text.Wrap
            font.family: Theme.uiFont
            font.pixelSize: 13
            color: Theme.text
        }

        Text {
            Layout.fillWidth: true
            Layout.topMargin: 2
            visible: control.detail.length > 0
            text: control.detail
            wrapMode: Text.Wrap
            elide: Text.ElideMiddle
            maximumLineCount: 3
            font.family: Theme.uiFont
            font.pixelSize: 11
            color: Theme.textFaint
        }
    }

    footer: Item {
        implicitHeight: 60

        RowLayout {
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            spacing: 8

            AppButton {
                visible: control.cancelText.length > 0
                text: control.cancelText
                onClicked: {
                    control.close();
                    control.cancelClicked();
                }
            }

            AppButton {
                visible: control.secondaryText.length > 0
                text: control.secondaryText
                destructive: control.secondaryDestructive
                onClicked: {
                    control.close();
                    control.secondaryClicked();
                }
            }

            AppButton {
                visible: control.primaryText.length > 0
                text: control.primaryText
                primary: true
                onClicked: {
                    control.close();
                    control.primaryClicked();
                }
            }
        }
    }
}
