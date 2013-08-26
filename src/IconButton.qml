import QtQuick 1.1

Image {
    property int iconSize
    sourceSize.width: iconSize
    sourceSize.height: iconSize
    width: iconSize
    height: iconSize

    signal clicked()

    MouseArea {
        id: mousearea
        anchors.fill: parent
    }

    Component.onCompleted: {
        mousearea.clicked.connect(clicked)
    }
}