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
                    //const folderPath = LoadFileDialog.openFolderDialog();
                    //console.log("Path Selected: "+folderPath);
                    stackView.push(Qt.resolvedUrl("qrc:/QML/setupscreen.qml"))
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
                    font.pointSize: 14
                    width: projectGrid.cellWidth-20
                    height: projectGrid.cellHeight-20

                    Label {
                        text: "Project " + (index + 1)
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 20  // optional: spacing from top
                        horizontalAlignment: Text.AlignHCenter
                    }

                    background: Rectangle {
                        color: mouseArea.containsMouse ? "#357ABD" : "#4A90E2"
                        radius: 20
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            console.log("Button clicked!")
                        }
                    }
                }
            }
        }
    }

    onClosing: {
        controller.stopServer();
        Qt.quit();
    }
}
