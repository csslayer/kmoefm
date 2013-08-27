#include "themeiconprovider.h"
#include <QIcon>
#include <KDebug>
#include <KIcon>
#include <KIconEffect>

ThemeIconProvider::ThemeIconProvider(): QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{

}

QPixmap ThemeIconProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    QString name;
    QIcon::Mode mode = QIcon::Normal;
    QIcon::State state = QIcon::Off;
    int slash = id.indexOf('/');
    if (slash > 0) {
        name = id.section('/', 0, 0);
    } else {
        name = id;
    }

    QString modeString = id.section('/', 1, 1);
    QString stateString = id.section('/', 2, 2);
    if (modeString == "disabled") {
        mode = QIcon::Disabled;
    }
    if (modeString == "active") {
        mode = QIcon::Active;
    }
    if (stateString == "on") {
        state = QIcon::On;
    }
    QPixmap pixmap = KIcon(name).pixmap(requestedSize, mode, state);
    if (size) {
        *size = pixmap.size();
    }
    return pixmap;
}
