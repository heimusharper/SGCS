/*
 * This file is part of the SGCS distribution (https://github.com/heimusharper/SGCS).
 * Copyright (c) 2020 Andrey Stepanov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.12
import sgcs 1.0


RowLayout {
    id: control
    Layout.fillWidth: true
    signal activated(bool is_active)

    SerialConnectionSelectorListModel {
        id: serialConnection
        onBaudrateChanged: {
            combox.currentIndex = combox.find(serialConnection.baudrate.toString())
        }
        onSerialErrorChanged: {
            if (serialError)
            {
                serialErrorDialog.text = serialError
                serialErrorDialog.visible = true
            }
        }
    }

    MessageDialog {
        id: serialErrorDialog
        title: qsTr("Serial connection error?")
        icon: StandardIcon.Warning
        standardButtons: MessageDialog.Ok

        onApply: {
        }
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

        RowLayout {
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
                    id: combox
                    model: ["9600", "19200", "38400", "57600", "115200"]
                    enabled: !serialConnection.connected
                    currentIndex: find(serialConnection.baudrate.toString())
                    onCurrentTextChanged: {
                          serialConnection.setBaudrate(parseInt(combox.currentText))
                    }
                }
                Rectangle {
                    enabled: !serialConnection.connected
                }
                Button {
                    text: serialConnection.connected ? qsTr("Close") : qsTr("Connect")
                    onClicked: serialConnection.setConnected(!serialConnection.connected)
                }
                Button {
                    text: qsTr("Create")
                    onClicked: {
                        var component = Qt.createComponent("UavInstance.qml");
                        if (component.status === Component.Ready) {
                            var object = component.createObject(rootContainerSerial)
                        }
                    }
                }
            }
            ColumnLayout {
                id: rootContainerSerial
            }
        }
    }
    onActivated: {
        serialConnection.setCheck(is_active)
        combox.currentIndex = combox.find(serialConnection.baudrate.toString())
    }
}
