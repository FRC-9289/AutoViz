import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: robotSettings
    color: "#121212"

    width: 400;
    height: 300;

    ColumnLayout {
        id: columnLayout
        width: parent.width;
        height: parent.height;
        Label {
            text: "Robot Settings"
            color: "#B3E5FC";
            font.pixelSize: 24;
            font.weight: Font.Bold;
            Layout.alignment: Qt.AlignCenter;
        }
        Label {
            text: "Module Type"
            color: "#B3E5FC"
            font.pixelSize: 20;
            Layout.margins: 10;
        }
        ComboBox {
            id: moduleType
            Layout.fillWidth: true
            model: ["MK4i", "MK4", "L2", "Custom"]
            currentIndex: 0
            onCurrentTextChanged: moduleType = currentText
            palette.highlight: "#B3E5FC"
            palette.buttonText: "white"
            palette.text: "white"
            palette.base: "#1E1E1E"
            background: Rectangle {
                color: "#1E1E1E"
                radius: 4
            }
            width: 100;
            height: 50;
            Layout.margins: 10;
        }
        Label {
            text: "Starting Heading"
            color: "#B3E5FC"
            font.pixelSize: 20;
            Layout.margins: 10;
        }
        TextArea {
            id: startHeading
            Layout.margins: 10;
        }


    }
}
