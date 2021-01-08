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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0


// main window
ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("SGCS")
    objectName: "root"

    signal deactivateAll
    onDeactivateAll: {
        connection.activated(false)
    }

    Connection {
        id: connection
    }

    MapView {
        id: mapView
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr("‹")
                onClicked: {
                    // deactivate all workflows
                    deactivateAll()
                    //
                    if (stack.depth > 1) {
                        stack.pop()
                    } else {
                        drawer.open()
                    }
                }
            }
            Label {
                text: "Title"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                text: qsTr("⋮")
                onClicked: menu.open()
            }
        }
    }

    StackView {
        id: stack
        objectName: "stack"
        initialItem: mapView
        anchors.fill: parent
    }

    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height

        Column {
            anchors.fill: parent
            id: drawerButtons
            ItemDelegate {
                text: qsTr("Home")
                width: parent.width
                onClicked: {
                    deactivateAll()
                    while (stack > 1) {
                        stack.pop()
                    }
                    drawer.close()
                }
            }
            ItemDelegate {
                text: qsTr("Connection")
                width: parent.width
                onClicked: {
                    stack.push(connection)
                    drawer.close()
                    connection.activated(true)
                }
            }
        }
    }
}
