#include "themeiconprovider.h"
#include <QIcon>

ThemeIconProvider::ThemeIconProvider(): QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
{

}

QPixmap ThemeIconProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    QPixmap pixmap = QIcon::fromTheme(id).pixmap(requestedSize);
    if (size) {
        *size = pixmap.size();
    }
    return pixmap;
}
