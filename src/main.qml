import QtQuick 1.1

Item {

    function formatTime(sec) {
        h = Math.floor(sec / 60 / 60);
        m = Math.floor(sec / 60) % 60;
        s = sec % 60;
        str = '';
        if (h > 0) {
            str = h.toString() + ':';
        }
        if (m < 10) {
            str += '0' + m.toString();
        } else {
            str += m.toString();
        }
        str += ':' + ((s < 10) ? '0' : '') + s.toString();
        return str;
    }

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
            id : artist
            elide: Text.ElideRight
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
            id : time
            anchors {
                top: album.bottom
                left: parent.left
                right: parent.right
                margins: 5
            }
            text : formatTime(controller.time) + ' / ' + controller.info.streamTime
            visible: controller.isPlaying
        }

        Rectangle {
            anchors {
                top: time.bottom
                left: parent.left
                right: parent.right
            }
            height: 2
            color: "grey"
            Rectangle {
                function calProgress() {
                    if (controller.info.streamLength > controller.time) {
                        return controller.time/controller.info.streamLength;
                    } else {
                        return 1;
                    }
                }

                color: "lightgrey"
                anchors {
                    top: parent.top
                    left: parent.left
                }
                height: 2
                width: parent.width * calProgress()
                visible: controller.isPlaying

                Behavior on width {
                    SequentialAnimation {
                        NumberAnimation { duration: 500 }
                    }
                }
            }
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
                top: time.bottom
                bottom: parent.bottom
            }
            Item {
                anchors.centerIn: parent
                width: like.width + playpause.width + next.width
                height: like.height
                Image {
                    id: like
                    source: "image://theme/favorites" + ((controller.info.favId.length > 0) ? "" :  "/disabled")
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
