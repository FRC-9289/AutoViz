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

    property real baseWidth: 477
    property real baseSpacing: 24.22265625
    property real robotBaseSize: baseSpacing
    property real spacing: (width / baseWidth) * baseSpacing / 2.0
    property double startHeading: 0;

    property bool simulationRecording: false
    property string projectName: ""
    property int frameIndex: 0
    property var projectData: ({})
    property double epsilon: 1e-8;
    property double startX: 6.5;
    property double startY: 4.0;
    property double errorLambda: 0.6;

    function moveAndRotateRobot(x_velocity, y_velocity, new_angle, duration) {
        let dt = duration * 1000;
        let dx = (x_velocity * spacing) * duration;
        let dy = (y_velocity * spacing) * duration;

        moveRotateAnim.animations[0].to = robot.item.x - dx;
        moveRotateAnim.animations[0].duration = dt;

        moveRotateAnim.animations[1].to = robot.item.y - dy;
        moveRotateAnim.animations[1].duration = dt;

        moveRotateAnim.animations[2].to = 90-new_angle;
        moveRotateAnim.animations[2].duration = dt;

        moveRotateAnim.running = false;
        moveRotateAnim.running = true;
    }
    function headingError(true_heading, heading){
        let diff = (true_heading - heading + 180) % 360 - 180;
        return diff;
    }
    function distanceError(true_x, true_y, pred_x, pred_y){
        let dist = Math.sqrt(
                    Math.pow(true_x-pred_x,2)+
                    Math.pow(true_y-pred_y,2)
                    );
        return dist
    }


    menuBar: MenuBar {
        Menu {
            title: "AutoViz"

            MenuItem {
                text: "Pref"
                onTriggered: {
                    settings.visible=true;
                }
            }
            MenuItem {
                text: "Open"
                onTriggered: console.log("Open clicked")
            }
            MenuItem {
                text: "Save"
                onTriggered: console.log("Save clicked")
            }
        }
    }




    Connections {
        target: controller
        function onCsvProcessed(result){
            projectData=result;
            console.log(result);
                        }
    }

    Timer {
        id: animationTimer
        interval: projectData.ts[frameIndex]*1000;
        repeat: true
        running: false
        onTriggered: { //Update timer
            if (frameIndex  == projectData.ts.length-1) { //Stop @ second last index
                stop()
                return
            }

            controller.updateRobot(projectData.v_x[frameIndex],projectData.v_y[frameIndex],projectData.omega[frameIndex],projectData.ts[frameIndex],Robot); //Update Robot State



            let startHeading = controller.radiansToDegrees(controller.getHeading(Robot)); //Updaate

            console.log("ANGLE: "+startHeading);

            let vxRelative=controller.getFieldRelativeVelocity(Robot)[0];
            let vyRelative=controller.getFieldRelativeVelocity(Robot)[1];
            let duration = projectData.ts[frameIndex];

            startX+=vxRelative*duration
            startY+=vyRelative*duration

            console.log("Heading error: "+headingError(180,startHeading));
            console.log("Distance error: "+distanceError(3,7,startX,startY));

            moveAndRotateRobot(vxRelative,vyRelative,startHeading,duration);


            frameIndex++;
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
            source: "qrc:/QML/robot.qml"
            anchors.centerIn: parent
        }
        Loader {
            id: settings
            source: "qrc:/QML/robotsettings.qml"
            visible: false
        }

        ParallelAnimation {
            id: moveRotateAnim
            running: false
            NumberAnimation { target: robot.item; property: "x"; to: 0; duration: 1000; easing.type: Easing.InOutQuad }
            NumberAnimation { target: robot.item; property: "y"; to: 0; duration: 1000; easing.type: Easing.InOutQuad }
            NumberAnimation { target: robot.item; property: "rotation"; to: 0; duration: 1000; easing.type: Easing.InOutQuad }
        }


        function updateRobotSize() {
            if (robot.item) {
                let scale = width / baseWidth
                robot.item.width = robotBaseSize * scale
                robot.item.height = robotBaseSize * scale
            }
        }

        onWidthChanged: {
            gridCanvas.requestPaint()
            updateRobotSize()
        }
        onHeightChanged: {
            gridCanvas.requestPaint()
            updateRobotSize()
        }

        Canvas {
            id: gridCanvas
            anchors.fill: parent
            onPaint: {
                let ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                ctx.strokeStyle = "rgba(255, 255, 255, 0.3)"
                ctx.lineWidth = 1
                for (let x = 0; x <= width; x += spacing) {
                    ctx.beginPath()
                    ctx.moveTo(x, 0)
                    ctx.lineTo(x, height)
                    ctx.stroke()
                }
                for (let y = 0; y <= height; y += spacing) {
                    ctx.beginPath()
                    ctx.moveTo(0, y)
                    ctx.lineTo(width, y)
                    ctx.stroke()
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
            pinch.dragAxis: Pinch.NoDrag
            onPinchUpdated: {
                const sensitivity = 0.1
                const zoomDelta = 1 + (pinch.scale - 1) * sensitivity
                zoomFactor = Math.max(minZoom, Math.min(maxZoom, zoomFactor * zoomDelta))
            }
            onPinchFinished: pinch.scale = 1
        }
    }

    MessageDialog {
        id: reminder
        text: "Don't forget to close window when Auton is finished running. We are extracting your Robot data"
        buttons: MessageDialog.Ok
    }

    Row {
        spacing: 8
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12

        Button {
            text: "+"
            onClicked: zoomFactor = Math.min(maxZoom, zoomFactor + zoomStep)
        }
        Button {
            text: "-"
            onClicked: zoomFactor = Math.max(minZoom, zoomFactor - zoomStep)
        }
        Label {
            text: Math.round(zoomFactor * 100) / 100 + "x"
            color: "white"
        }
    }

    Button {
        id: stopRecording
        text: "Stop Recording"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 20
        visible: false
        background: Rectangle {
            color: Qt.rgba(255, 0, 0, 0.5)
            radius: 10
        }
        onClicked: {
            stopRecording.visible = false
            controller.stopServer()
            var data = controller.processCSV(projectName);
        }
    }

    onClosing: controller.stopServer()

    Component.onCompleted: {
        console.log("Name of Project: " + projectName)
        console.log("Is Recording: " + simulationRecording);


        /*
        robot.item.frontLeft.rotation=45;
        robot.item.backRight.rotation=45;
        robot.item.frontRight.rotation=-45;
        robot.item.backLeft.rotation=-45;
        */

        if (simulationRecording) {
            stopRecording.visible = true
            reminder.open()
        } else {
            projectData = controller.getCSV(projectName)
            controller.setHeading(controller.degreesToRadians(45.0), Robot);

            startHeading=controller.radiansToDegrees(controller.getHeading(Robot));
            robot.item.rotation=90-startHeading;
            animationTimer.start()
        }

    }
}
