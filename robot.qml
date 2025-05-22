import QtQuick

Rectangle {
    id: robotBody
    width: 150
    height: 150
    color: "#5A5A5A"
    border.color: "white"
    border.width: 3
    radius: 10

    // Front Left wheel
    Rectangle {
        width: 8
        height: 30
        color: "black"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        rotation: 0
        transformOrigin: Item.Center
    }

    // Front Right wheel
    Rectangle {
        width: 8
        height: 30
        color: "black"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        rotation: 0
        transformOrigin: Item.Center
    }

    // Back Left wheel
    Rectangle {
        width: 8
        height: 30
        color: "black"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        rotation: 0
        transformOrigin: Item.Center
    }

    // Back Right wheel
    Rectangle {
        width: 8
        height: 30
        color: "black"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        rotation: 0
        transformOrigin: Item.Center
    }
}
