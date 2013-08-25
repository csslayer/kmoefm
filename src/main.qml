import QtQuick 1.1

Item {

    Image {
        anchors {
            left: parent.left
        }
        id: cover
        source: controller.info.cover
        height: 192
        width: 192
        smooth: true
    }

    Item {
        anchors {
            top: cover.top
            left: cover.right
            right: parent.right
            bottom: cover.bottom
        }

        Text {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            id : title
            elide: Text.ElideMiddle
            textFormat: Text.PlainText
            text: controller.info.title
            color: controller.info.songUrl.length > 0 ? controller.linkColor : controller.textColor
            font.underline: controller.info.songUrl.length > 0
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (controller.info.songUrl.length > 0) {
                        Qt.openUrlExternally(controller.info.songUrl);
                    }
                }
            }
        }

        Text {
            anchors {
                top: title.bottom
                left: title.left
                right: parent.right
            }
            id : artist
            elide: Text.ElideMiddle
            text: controller.info.artist
        }

        Text {
            anchors {
                top: artist.bottom
                left: title.left
                right: parent.right
            }
            id : album
            color: controller.info.albumUrl.length > 0 ? controller.linkColor : controller.textColor
            elide: Text.ElideMiddle
            text: controller.info.album
            textFormat: Text.PlainText
            font.underline: controller.info.albumUrl.length > 0
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (controller.info.albumUrl.length > 0) {
                        Qt.openUrlExternally(controller.info.albumUrl);
                    }
                }
            }
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
                top: album.bottom
                bottom: parent.bottom
            }
            Image {
                id: like
                source: "image://theme/favorites"
                sourceSize.width: width
                sourceSize.height: height
                anchors {
                    verticalCenter: parent.verticalCenter
                }

                width: 32
                height: 32

                MouseArea {
                    anchors.fill: parent
                    onClicked: controller.like()
                }
            }

            Image {
                id: next
                source: "image://theme/media-skip-forward"
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: playpause.right
                }

                sourceSize.width: width
                sourceSize.height: height
                width: 32
                height: 32

                MouseArea {
                    anchors.fill: parent
                    onClicked: controller.playNext()
                }
            }

            Image {
                id: playpause
                source: controller.isPaused ? "image://theme/media-playback-start" :  "image://theme/media-playback-pause"
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: like.right
                }

                sourceSize.width: width
                sourceSize.height: height
                width: 32
                height: 32

                MouseArea {
                    anchors.fill: parent
                    onClicked: controller.playPause()
                }
            }
        }
    }
}
