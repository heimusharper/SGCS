import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import sgcs 1.0

RowLayout {
    Layout.fillWidth: true
    signal activated(bool is_active)

    SerialConnectionSelectorListModel {
        id: serialConnection
    }

    GroupBox {
        label: Label {
               text: qsTr("Serial")
           }
        GridLayout {
            columns: 2
            anchors.fill: parent
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
