import QtQuick 2.12
import QtQuick.Controls 2.12
import Ru.SGCS 1.0 as SGCS

Page {
    title: qsTr("Tracker")

    ListView {
        id: listView
        anchors.fill: parent
        model: SGCS.PluginLoaderListModel

        delegate: Item {
            implicitHeight: text.height
            width: listView.width
            Text {
                id: text
                text: model.name + " " + model.visible
            }

            MouseArea {
                anchors.fill: text
                onClicked: {
                    model.quantity *= 2;
                }
            }
        }
    }
}
