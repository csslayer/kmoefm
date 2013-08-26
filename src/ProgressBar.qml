import QtQuick 1.1

Item {
    property int time;
    property int totalTime;
    property color windowColor;
    height: childrenRect.height
    anchors.margins: 5

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

    Text {
        id : timeText
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        text : formatTime(time) + ' / ' + formatTime(totalTime)
    }

    Rectangle {
        id: bar
        anchors {
            top: timeText.bottom
            left: parent.left
            right: parent.right
        }
        height: 2
        color: Qt.darker(windowColor, 1.5)
        Rectangle {
            function calProgress() {
                if (totalTime > time && totalTime > 0) {
                    return time/totalTime;
                } else {
                    return 1;
                }
            }

            color: Qt.lighter(windowColor, 1.5)
            anchors {
                top: parent.top
                left: parent.left
            }
            height: 2
            width: parent.width * calProgress()
        }
    }
}
