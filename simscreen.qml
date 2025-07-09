import QtQuick.Window 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs


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
    property real spacing: (width / baseWidth) * baseSpacing/2.0

    property bool simulationRecording: false;
    property string projectName: "";

    Component.onCompleted: {
        //if(simulationRecording){
        //    stopRecording.visible = true;
        //}
        console.log("Name of Project: "+projectName); //Not working
        console.log("Is Recording: "+simulationRecording); //Not working
        if(simulationRecording){
            stopRecording.visible = true;
            reminder.open();

        }
        else {
            var projectData = controller.getCSV(projectName);
            console.log("TIMESTAMP: "+projectData.ts);
            console.log("V_X: "+projectData.v_x);
            console.log("V_Y: "+projectData.v_y);
            console.log("OMEGA: "+projectData.omega);

            for(let i=0;i<projectData.ts.length;i++) {
                Robot.updateHeading(projectData.omega[i],projectData.ts[i]); //TODO Error
            }
            console.log(Robot.heading)

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

        Rectangle {
            color: "#121212"
            anchors.fill: parent
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

        function setRobotRotation(degrees) {
            if (robot.item) {
                robot.item.rotation = degrees
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
        PinchArea {
            anchors.fill: parent
            pinch.target: zoomContainer
            pinch.minimumScale: minZoom
            pinch.maximumScale: maxZoom
            pinch.dragAxis: Pinch.NoDrag  // disables panning

            onPinchUpdated: {
                const sensitivity = 0.1;  // Lower = slower zoom, try 0.1 - 0.3
                const zoomDelta = 1 + (pinch.scale - 1) * sensitivity;

                zoomFactor = Math.max(minZoom, Math.min(maxZoom, zoomFactor * zoomDelta));
            }

            onPinchFinished: {
                pinch.scale = 1  // reset pinch scale for next gesture
            }
        }
    }

    MessageDialog {
        id: reminder
        text: "Don't forget to close window when Auton is finished running. We are extracting your Robot data"
        buttons: MessageDialog.Ok
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


    Button {
        id: stopRecording
        text: "Stop Recording"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 20
        visible: false //Set to true if simulation recording
        background: Rectangle {
            color: Qt.rgba(255,0,0,0.5)
            radius: 10
        }
        onClicked: {
            stopRecording.visible = false;
            controller.stopServer();
            var projectData = controller.getCSV(projectName);
            console.log("TIMESTAMP: "+projectData.ts);
            console.log("V_X: "+projectData.v_x);
            console.log("V_Y: "+projectData.v_y);
            console.log("OMEGA: "+projectData.omega);
        }
    }

    onClosing: {
        controller.stopServer();
    }

}
