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
                enabled: !serialConnection.connected
            }
            ComboBox {
                model: serialConnection
                textRole: "name"
                currentIndex: serialConnection.current
                enabled: !serialConnection.connected
                onCurrentIndexChanged: {
                    serialConnection.setCurrent(currentIndex)
                }
            }
            Label {
                text: qsTr("Baudrate")
                enabled: !serialConnection.connected
            }
            ComboBox {
                model: ["9600", "57600"]
                currentIndex: find(serialConnection.baudrate.toString())
                enabled: !serialConnection.connected
            }
            Rectangle {
                enabled: !serialConnection.connected
            }
            Button {
                text: serialConnection.connected ? qsTr("Close") : qsTr("Connect")
                onClicked: serialConnection.setConnected(!serialConnection.connected)
            }
        }
    }
    onActivated: {
        serialConnection.setCheck(is_active)
    }
}
