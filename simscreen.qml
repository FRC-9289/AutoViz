import QtQuick.Window 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    width: 1000
    height: 800
    visible: true
    title: "Simulation Screen 🤖"

    Material.theme: Material.Dark
    Material.accent: "#39FF14"

    property real zoomFactor: 1.0
    property real zoomStep: 0.1
    property real minZoom: 0.2
    property real maxZoom: 5.0

    // Base width and spacing ratio
    property real baseWidth: 477
    property real baseSpacing: 24.22265625
    property real robotBaseSize: baseSpacing  // size in px for 1m robot at base width

    // Calculate spacing proportionally to current width
    property real spacing: (width / baseWidth) * baseSpacing

    Component.onCompleted: {
        const robot = new Robot();
        console.log(robot.getRobotOmega());
    }

    Connections {
        onModuleDataReceived: {
            console.log(obj)
        }
    }

    Item {
        id: zoomContainer
        anchors.fill: parent
        transform: Scale {
            xScale: zoomFactor
            yScale: zoomFactor
            origin.x: zoomContainer.width / 2
            origin.y: zoomContainer.height / 2
        }

        Image {
            id: baseImage
            anchors.fill: parent
            source: "qrc:/SimScreen/2025 REEFSCAPE Transparent Background.png"
            fillMode: Image.PreserveAspectFit
        }

        Loader {
            id: robot
            source: "robot.qml"
            anchors.centerIn: parent
            onLoaded: {
                updateRobotSize();
            }
        }

        function updateRobotSize() {
            if (robot.item) {
                let scale = width / baseWidth;
                robot.item.width = robotBaseSize * scale;
                robot.item.height = robotBaseSize * scale;
            }
        }

        // Redraw grid and resize robot when size changes
        onWidthChanged: {
            gridCanvas.requestPaint();
            updateRobotSize();
        }
        onHeightChanged: {
            gridCanvas.requestPaint();
            updateRobotSize();
        }

        Canvas {
            id: gridCanvas
            anchors.fill: parent

            onPaint: {
                let ctx = getContext("2d");
                ctx.clearRect(0, 0, width, height);

                ctx.strokeStyle = "rgba(255, 255, 255, 0.3)";
                ctx.lineWidth = 1;

                for (let x = 0; x <= width; x += spacing) {
                    ctx.beginPath();
                    ctx.moveTo(x, 0);
                    ctx.lineTo(x, height);
                    ctx.stroke();
                }

                for (let y = 0; y <= height; y += spacing) {
                    ctx.beginPath();
                    ctx.moveTo(0, y);
                    ctx.lineTo(width, y);
                    ctx.stroke();
                }
            }

            Component.onCompleted: requestPaint()
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        onWheel: (wheel) => {
            const delta = wheel.angleDelta.y > 0 ? zoomStep : -zoomStep;
            zoomFactor = Math.max(minZoom, Math.min(maxZoom, zoomFactor + delta));
        }
    }

    Row {
        spacing: 8
        anchors {
            right: parent.right
            top: parent.top
            margins: 12
        }

        Button {
            text: "+"
            onClicked: zoomFactor = Math.min(maxZoom, zoomFactor + zoomStep)
        }
        Button {
            text: "-"
            onClicked: zoomFactor = Math.max(minZoom, zoomFactor - zoomStep)
        }
        Label {
            text: Math.round(zoomFactor*100)/100+ "x"
            color: "white"
        }
    }

    onClosing: {
        NTManager.stopServer();
    }
}
