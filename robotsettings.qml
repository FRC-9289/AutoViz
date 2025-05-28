import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    width: 400
    height: 400
    visible: true
    title: "Settings ⛭"

    Material.theme: Material.Dark
    Material.accent: "#39FF14"
    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            anchors.fill: parent

            Label {
                text: "Robot Settings"
                font.pixelSize: 24
                Layout.alignment: Qt.AlignHCenter
            }

            GridView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                cellWidth: 160
                cellHeight: 60
                model: 12

                delegate: Rectangle {
                    width: 140
                    height: 50
                    Text {
                        anchors.centerIn: parent
                        text: "Hello world " + index
                        color: "white"

                    }
                }
            }
        }
    }
}
