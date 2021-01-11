import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import sgcs 1.0


RowLayout {
    id: control
    Layout.fillWidth: true
    signal activated(bool is_active)

    SerialConnectionSelectorListModel {
        id: serialConnection
    }

    ColumnLayout {

        RowLayout {
                Label{
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    text: qsTr("Serial")
                }
                ToolButton {
                    id: buttonObj
                    text: qsTr("x")
                    Layout.alignment: Qt.AlignRight
                    onClicked: control.destroy()
                }
            }

        GridLayout {
            id: xe
            columns: 2
            Label {
                text: qsTr("Serial")
            }
            ComboBox {
                model: serialConnection
                textRole: "name"
                currentIndex: serialConnection.current
                onCurrentIndexChanged: {
                    serialConnection.setCurrent(currentIndex)
                }
            }
            Label {
                text: qsTr("Baudrate")
            }
            ComboBox {

            }
            Rectangle {

            }
            Button {
                text: qsTr("Connect")
            }
        }
    }
    onActivated: {
        serialConnection.setCheck(is_active)
    }
}
