import QtQuick 1.1

Item {
    id: root
    property int iconSize: 48
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

    Item {
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        id: cover
        height: 192
        width: 192

        Connections {
            id: coverChange
            property bool m : true;
            target: controller
            onInfoChanged: {
                coverChange.m = !coverChange.m;
                if (coverChange.m) {
                    coverImage2.source = controller.info.cover
                } else {
                    coverImage1.source = controller.info.cover
                }
            }
        }

        states: [
            State {
                name: "cover1"; when: coverChange.m
                PropertyChanges { target: coverImage1; opacity: 0.0 }
                PropertyChanges { target: coverImage2; opacity: 1.0 }
            },
            State {
                name: "cover2"; when: !coverChange.m
                PropertyChanges { target: coverImage1; opacity: 1.0 }
                PropertyChanges { target: coverImage2; opacity: 0.0 }
            }]

        Image {
            id: coverImage1
            anchors.fill: parent
            smooth: true
            asynchronous: true
            Behavior on opacity {
                SequentialAnimation {
                    PauseAnimation { duration: 500 }
                    NumberAnimation { duration: 1000 }
                }
            }
        }

        Image {
            id: coverImage2
            anchors.fill: parent
            asynchronous: true
            smooth: true
            Behavior on opacity {
                NumberAnimation { duration: 1000 }
            }
        }

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
            margins: 15
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
            font.pointSize: controller.fontSize * 1.5
            elide: Text.ElideRight
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
            id : album
            color: controller.info.albumUrl.length > 0 ? controller.linkColor : controller.textColor
            elide: Text.ElideRight
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

        Text {
            anchors {
                top: album.bottom
                left: parent.left
                right: parent.right
                margins: 5
            }
            id : artist
            elide: Text.ElideRight
            text: controller.info.artist
        }

        ProgressBar {
            id: progressBar
            anchors {
                left: parent.left
                right: parent.right
                top: artist.bottom
            }
            time: controller.time
            totalTime: controller.info.streamLength
            windowColor: controller.windowColor
        }


        Rectangle {
            color: "red"
            anchors {
                left: parent.left
                right: parent.right
                top: progressBar.bottom
                bottom: parent.bottom
            }
            Item {
                anchors.centerIn: parent
                width: (root.iconSize * 1.5) * 4
                height: (root.iconSize * 1)

                IconButton {
                    id: like
                    source: "image://theme/favorites" + ((controller.info.favId.length > 0) ? "" :  "/disabled")
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: root.iconSize * 0.5
                        rightMargin: root.iconSize * 0.5
                    }

                    iconSize: root.iconSize

                    onClicked: controller.like(false)
                }

                IconButton {
                    id: likeAlbum
                    source: "image://theme/media-optical-mixed-cd" + ((controller.info.favAlbum.length > 0) ? "" :  "/disabled")
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: like.right
                        leftMargin: root.iconSize * 0.5
                        rightMargin: root.iconSize * 0.5
                    }
                    iconSize: root.iconSize
                    onClicked: controller.like(true)
                }

                IconButton {
                    id: next
                    source: "image://theme/media-skip-forward"
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: playpause.right
                        leftMargin: root.iconSize * 0.5
                        rightMargin: root.iconSize * 0.5
                    }
                    iconSize: root.iconSize
                    onClicked: controller.playNext()
                }

                IconButton {
                    id: playpause
                    source: controller.isPaused ? "image://theme/media-playback-start" :  "image://theme/media-playback-pause"
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: likeAlbum.right
                        leftMargin: root.iconSize * 0.5
                        rightMargin: root.iconSize * 0.5
                    }

                    iconSize: root.iconSize
                    onClicked: controller.playPause()
                }
            }
        }
    }
}
