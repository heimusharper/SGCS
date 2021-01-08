import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Ru.SGCS 1.0 as SGCS

Page {
    title: qsTr("Tracker")
    anchors.fill: parent

    signal activated(bool is_active)

    onActivated: {
        SGCS.SerialConnectionFabric.setCheck(is_active)
    }

    ColumnLayout {
        GroupBox {
            label: CheckBox {
                   id: checkBoxSerialConnection
                   checked: false
                   text: qsTr("Serial")
               }
            GridLayout {
                columns: 2
                anchors.fill: parent
                Label {
                    text: qsTr("Serial")
                }
                ComboBox {
                    model: SGCS.SerialConnectionFabric
                    textRole: "name"
                    currentIndex: SGCS.SerialConnectionFabric.current
                    onCurrentIndexChanged: {
                        SGCS.SerialConnectionFabric.setCurrent(currentIndex)
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
        GroupBox {
            label: CheckBox {
                   id: checkBoxNetworkConnection
                   checked: true
                   text: qsTr("Network")
               }
            GridLayout {
                columns: 2
                anchors.fill: parent
                Label {
                    text: qsTr("IP")
                }
                ComboBox {

                }
                Label {
                    text: qsTr("Port")
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
    }
}
