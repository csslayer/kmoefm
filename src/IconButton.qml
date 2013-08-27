import QtQuick 1.1

Item {
    property int iconSize
    property string mode: ""
    property string icon: ""
    width: iconSize
    height: iconSize
    Image {
        id: image
        source: "image://theme/" + parent.icon + "/" + (mode.length > 0 ? mode : (mousearea.containsMouse ? "active" : ""))
        smooth: true
        anchors.centerIn: parent
        sourceSize.width: iconSize
        sourceSize.height: iconSize
        width: iconSize * (mousearea.pressed ? 0.9 : 1.0)
        height: iconSize * (mousearea.pressed ? 0.9 : 1.0)
    }

    signal clicked()

    MouseArea {
        id: mousearea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            console.log(image.source)
        }
    }

    Component.onCompleted: {
        mousearea.clicked.connect(clicked)
    }
}
