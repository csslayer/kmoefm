import QtQuick 1.1

Item {
    Item {
        z: 1
        anchors.fill: parent
        id: favOverlay
        opacity: 0.0
        Rectangle {
            anchors.fill: parent
            opacity: 0.4
            color: "grey"

        }
        Rectangle {
            width: favText.width + Math.min(favText.width * 0.5, favText.height * 0.5)
            height: favText.height + Math.min(favText.width * 0.5, favText.height * 0.5)
            anchors.centerIn: parent
            opacity: 0.8
            color: "white"
            radius: Math.min(favText.width * 0.5, favText.height * 0.5)
        }
        Text {
            id: favText
            anchors.centerIn: parent
            text: i18n("Favorite Added")
            color: "black"
        }


        Connections {
            target: controller
            onFavoriteAdded : favAnimation.start()
        }

        SequentialAnimation {
            id: favAnimation
            NumberAnimation { target: favOverlay; property: "opacity"; to: 1.0; duration: 250 }
            PauseAnimation  { duration: 1000 }
            NumberAnimation { target: favOverlay; property: "opacity"; to: 0.0; duration: 250 }
        }
    }

    Image {
        anchors {
            left: parent.left
        }
        id: cover
        source: controller.info.cover
        asynchronous: true
        height: 192
        width: 192
        smooth: true
        MouseArea {
            anchors.fill: parent
            onClicked: controller.playPause()
        }

        Rectangle {
            id: coverOverlay
            anchors.fill: parent
            opacity: 0.0
            color: "grey"

            Image {
                source: "image://theme/media-playback-pause"
                anchors.centerIn: parent

                sourceSize.width: width
                sourceSize.height: height
                width: 32
                height: 32
            }

            states: [
                State {
                    name: "paused"; when: controller.isPaused
                    PropertyChanges { target: coverOverlay; opacity: 0.5 }
                },
                State {
                    name: "play"; when: !controller.isPaused
                    PropertyChanges { target: coverOverlay; opacity: 0.0 }
                }]
            transitions: Transition {
                NumberAnimation { properties: "opacity"; easing.type: Easing.Linear ; }
            }
        }
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
                margins: 5
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
                left: parent.left
                right: parent.right
                margins: 5
            }
            id : artist
            elide: Text.ElideMiddle
            text: controller.info.artist
        }

        Text {
            anchors {
                top: artist.bottom
                left: parent.left
                right: parent.right
                margins: 5
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
            Item {
                anchors.centerIn: parent
                width: like.width + playpause.width + next.width
                height: like.height
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
}
