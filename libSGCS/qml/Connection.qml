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
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import SGCS 1.0

Page {
    title: qsTr("Tracker")
    anchors.fill: parent

    signal activated(bool is_active)

    onActivated: {
        SerialConnectionFabric.setCheck(is_active)
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
                    model: SerialConnectionFabric
                    textRole: "name"
                    currentIndex: SerialConnectionFabric.current
                    onCurrentIndexChanged: {
                        SerialConnectionFabric.setCurrent(currentIndex)
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
