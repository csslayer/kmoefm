#include "themeiconprovider.h"
#include <QIcon>

ThemeIconProvider::ThemeIconProvider(): QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{

}

QPixmap ThemeIconProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    QString name;
    QIcon::Mode mode = QIcon::Normal;
    int slash = id.indexOf('/');
    if (slash > 0) {
        name = id.section('/', 0, 0);
    } else {
        name = id;
    }

    if (id.section('/', 1, -1) == "disabled") {
        mode = QIcon::Disabled;
    }
    QPixmap pixmap = QIcon::fromTheme(name).pixmap(requestedSize, mode);
    if (size) {
        *size = pixmap.size();
    }
    return pixmap;
}
