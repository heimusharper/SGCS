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
import sgcs 1.0

Page {

    id: rootPage
    title: qsTr("Tracker")
    anchors.fill: parent

    property bool isActive: false;
    signal activated(bool is_active)


    ScrollView {
        anchors.fill: parent
        ColumnLayout {

            RowLayout {
                Label {
                    text: qsTr("New connection")
                    Layout.fillWidth: true
                }
                ComboBox {
                    id: methodSelector
                    model: ["Serial"]
                    currentIndex: 0
                    Layout.fillWidth: true
                }
                Button {
                    text: qsTr("Append")
                    Layout.fillWidth: true
                    onClicked: {
                        if (methodSelector.currentIndex === 0){
                            var component = Qt.createComponent("ConnectionSerial.qml");
                            if (component.status === Component.Ready) {
                                var object = component.createObject(rootContainer);
                                object.activated(isActive)
                            }
                            checkWidth()
                        }
                    }
                }
            }
            GridLayout {
                id: rootContainer
                columns: 1
            }
        }
    }
    onWidthChanged: checkWidth()

    onActivated: {
        for(var i = 0; i < rootContainer.children.length; ++i) {
            rootContainer.activated(is_active);
        }
        isActive = is_active;
    }

    function checkWidth(){
        var maxWidth = 0;
        for(var i = 0; i < rootContainer.children.length; ++i) {
            maxWidth = Math.max(maxWidth, rootContainer.children[i].width);
        }
        if (maxWidth > 0 && maxWidth < rootPage.width) {
            rootContainer.columns = Math.floor(rootPage.width / maxWidth)
            console.log("set " + rootContainer.columns)
        } else {
            rootContainer.columns = 1
        }
    }
}
