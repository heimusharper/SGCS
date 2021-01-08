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

    PluginLoader {
        id: pluginLoader
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
