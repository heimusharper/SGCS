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
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

import sgcs 1.0


// main window
ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    flags: Qt.FramelessWindowHint

    signal deactivateAll
    onDeactivateAll: {
        connection.activated(false)
    }


    property int previousX
    property int previousY
    property int headerHeight: 40

    WindowConf {
        id: conf
    }

    title: qsTr("SGCS") + " v" + conf.version

    Connection {
        id: connection
    }

    MapView {
        id: mapView
    }


    MessageDialog {
        id: checkCloseDialog
        title: qsTr("Realy close?")
        icon: StandardIcon.Warning
        text: qsTr("Do you realy want to close application?")
        standardButtons: MessageDialog.Yes | MessageDialog.Cancel

        onYes: {
            Qt.quit()
        }
        onRejected: {

        }
    }



    header: ToolBar {
        id: toolBarObj
        height: headerHeight
        RowLayout {
            anchors.fill: parent
            spacing: 0
            ToolButton {
                id: logoButton
                Layout.fillHeight: true
                hoverEnabled: true
                background: Image {
                            width: toolBarObj.height
                            height: toolBarObj.height
                            anchors.fill: logoButtonRect
                            source: "qrc:/logo.png"
                            opacity: (logoButton.hovered || logoButton.checked || logoButton.highlighted) ? 0.5 : 1.
                        }

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

                MouseArea {
                    height: 5
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    cursorShape: Qt.SizeVerCursor

                    onPressed: {
                        previousY = mouseY
                    }
                    onMouseYChanged: {
                        var dy = mouseY - previousY
                        window.setY(window.y + dy)
                        window.setHeight(window.height - dy)
                    }
                }
            }

            Label {
                text: "Title"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true


                MouseArea {
                    id: topMover
                    height: 5
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    cursorShape: Qt.SizeVerCursor

                    onPressed: {
                        previousY = mouseY
                    }
                    onMouseYChanged: {
                        var dy = mouseY - previousY
                        window.setY(window.y + dy)
                        window.setHeight(window.height - dy)
                    }
                }

                MouseArea {
                    anchors {
                        top: topMover.top
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    cursorShape: Qt.SizeAllCursor
                    onPressed: {
                        previousX = mouseX
                        previousY = mouseY
                    }
                    onMouseXChanged: {
                        var dx = mouseX - previousX
                        window.setX(window.x + dx)
                    }
                    onMouseYChanged: {
                        var dy = mouseY - previousY
                        window.setY(window.y + dy)
                    }
                }
            }

            ToolButton {
                id: minimizeControl
                text: "_"
                onClicked: {
                    window.showMinimized()
                }
                hoverEnabled: true
                background: Rectangle {
                          implicitWidth: toolBarObj.height
                          implicitHeight: toolBarObj.height
                          opacity: (minimizeControl.hovered || minimizeControl.checked || minimizeControl.highlighted) ? 0.5 : 0.
                      }

                MouseArea {
                    height: 5
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    cursorShape: Qt.SizeVerCursor

                    onPressed: {
                        previousY = mouseY
                    }
                    onMouseYChanged: {
                        var dy = mouseY - previousY
                        window.setY(window.y + dy)
                        window.setHeight(window.height - dy)
                    }
                }
            }

            ToolButton {
                id: maximizeControl
                text: (window.visibility === ApplicationWindow.Windowed) ? "❒" : "▣"
                onClicked: {
                    if ((window.visibility === ApplicationWindow.Windowed))
                        window.showMaximized()
                    else
                        window.showNormal()
                }
                hoverEnabled: true

                background: Rectangle {
                          implicitWidth: toolBarObj.height
                          implicitHeight: toolBarObj.height
                          opacity: (maximizeControl.hovered || maximizeControl.checked || maximizeControl.highlighted) ? 0.5 : 0.
                      }

                MouseArea {
                    height: 5
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    cursorShape: Qt.SizeVerCursor

                    onPressed: {
                        previousY = mouseY
                    }
                    onMouseYChanged: {
                        var dy = mouseY - previousY
                        window.setY(window.y + dy)
                        window.setHeight(window.height - dy)
                    }
                }
            }

            ToolButton {
                id: closeControl
                text: "✖"
                onClicked: {
                    checkCloseDialog.visible = true
                }
                hoverEnabled: true

                background: Rectangle {
                          implicitWidth: toolBarObj.height
                          implicitHeight: toolBarObj.height
                          color: (closeControl.hovered || closeControl.checked || closeControl.highlighted) ? "#ff200f" : "#ff4d40"
                      }

                MouseArea {
                    height: 5
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    cursorShape: Qt.SizeVerCursor

                    onPressed: {
                        previousY = mouseY
                    }
                    onMouseYChanged: {
                        var dy = mouseY - previousY
                        window.setY(window.y + dy)
                        window.setHeight(window.height - dy)
                    }
                }
                MouseArea {
                    width: 5
                    anchors {
                        top: parent.top
                        right: parent.right
                        bottom: parent.bottom
                    }
                    cursorShape: Qt.SizeHorCursor

                    onPressed: {
                        previousX = mouseX
                    }
                    onMouseXChanged: {
                        var dx = mouseX - previousX
                        //window.setX(window.y + dx)
                        window.setWidth(window.width + dx)
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        StackView {
            id: stack
            objectName: "stack"
            initialItem: mapView
            anchors.fill: parent
        }
        MouseArea {
            width: 5
            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }
            cursorShape: Qt.SizeHorCursor

            onPressed: {
                previousX = mouseX
            }
            onMouseXChanged: {
                var dx = mouseX - previousX
                //window.setX(window.y + dx)
                window.setWidth(window.width + dx)
            }
        }

        MouseArea {
            height: 5
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            cursorShape: Qt.SizeVerCursor

            onPressed: {
                previousY = mouseY
            }
            onMouseYChanged: {
                var dy = mouseY - previousY
                window.setHeight(window.height + dy)
            }
        }
        Rectangle {
            width: 15
            height: 15
            anchors {
                bottom: parent.bottom
                right: parent.right
            }
            color: "#00ffffff"
            Canvas{
                anchors.fill:parent
                onPaint:{
                     var context = getContext("2d");
                    context.beginPath();
                    context.moveTo(15, 0);
                    context.lineTo(15, 15);
                    context.lineTo(0, 15);
                    context.closePath();
                    context.fillStyle = "#FFCC00";
                    context.fill();
                }
            }
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.SizeFDiagCursor
                onPressed: {
                    previousX = mouseX
                    previousY = mouseY
                }
                onMouseXChanged: {
                    var dx = mouseX - previousX
                    window.setWidth(window.width + dx)
                }
                onMouseYChanged: {
                    var dy = mouseY - previousY
                    window.setHeight(window.height + dy)
                }
            }
        }
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
