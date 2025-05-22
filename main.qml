import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs


ApplicationWindow {
    id: window
    visible: true
    width: 1200
    height: 720
    title: "AutoViz - Project Launcher"

    Material.theme: Material.Dark
    Material.accent: "#39FF14"

    /*Connections {
        target: NTManager
        function onModuleDataReceived(data) {
            console.log("JSON Received:", JSON.stringify(data))
        }
    }
    */

    /*FolderDialog {
        id: fileDialog
        title: "Select a Folder"
        onAccepted: {
            console.log("Selected folder: " + fileDialog.currentFolder) // ✅ correct
        }
        onRejected: {
            console.log("Dialog canceled")
        }
    }
    */

    StackView {
        id: stackView
        anchors.fill: parent
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Sidebar
        ColumnLayout {
            width: 220
            spacing: 20
            anchors.margins: 20
            Layout.fillHeight: true


            Button {
                Layout.margins: 20
                text: " + New Project"
                font.family: "Poppins"
                font.pointSize: 14
                width: 200
                Layout.alignment: Qt.AlignTop
                onClicked: {
                    const folderPath = LoadFileDialog.openFolderDialog();
                    console.log("Path Selected: "+folderPath);
                    //NTManager.startServer()
                    //NTManager.connectToNT()
                    //NTManager.startPolling()
                    //stackView.push(Qt.resolvedUrl("qrc:/QML/simscreen.qml"))
                }

            }

            // Spacer to push button to top
            Item {
                Layout.fillHeight: true
            }
        }

        // Main project grid with padding wrapper
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            GridView {
                id: projectGrid
                anchors.fill: parent
                anchors.margins: 20
                model: 12
                cellWidth: 325
                cellHeight: 220
                clip: true
                interactive: true  // Enables scrolling
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AlwaysOn  // ← Shows the vertical scrollbar
                }

                delegate: Button {
                    text: "Project " + (index + 1)
                    font.family: "Poppins"
                    font.pointSize: 14
                    width: projectGrid.cellWidth-20
                    height: projectGrid.cellHeight-20

                    background: Rectangle {
                        color: "#1D2545"
                        radius: 20
                    }
                }
            }
        }
    }

    onClosing: {
        NTManager.stopServer();
        Qt.quit();
    }
}
