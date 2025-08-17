import QtQuick.Window 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    width: 1000
    height: 800
    visible: true
    title: "Simulation Screen 🤖"

    Material.theme: Material.Dark
    Material.accent: "#39FF14"

    Image {
        id: fieldImage
        source: "qrc:/SimScreen/reefscape.png"
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectCrop
    }

    Loader {
        source: "qrc:/QML/robot.qml"
    }
}
