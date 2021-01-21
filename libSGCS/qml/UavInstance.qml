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
    id: controlUV
    Layout.fillWidth: true

    UavObject {
        id: uavConnection
    }




    /* MessageDialog {
        id: serialErrorDialog
        title: qsTr("Serial connection error?")
        icon: StandardIcon.Warning
        standardButtons: MessageDialog.Ok

        onApply: {
        }
    }*/



    ColumnLayout {
        RowLayout {
            Label{
                id: uavID
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                text: (uavConnection.id < 0) ? qsTr("UAV") : qsTr("UAV") + " " + uavConnection.id.toString()
            }
            ToolButton {
                id: buttonObjRemoveUV
                text: qsTr("x")
                Layout.alignment: Qt.AlignRight
                onClicked: controlUV.destroy()
            }
        }
        ComboBox {
            id: combox
            model: [qsTr("empty")]
            enabled: !uavConnection.connected
            onCurrentTextChanged: {
            }
        }

        RowLayout {
            Button {
                text: uavConnection.connected ? qsTr("Close") : qsTr("Connect")
                onClicked:
                {
                    uavConnection.setConnected(!uavConnection.connected)
                    UavFabric.createObject(uavConnection)
                }
            }
        }
    }
}
