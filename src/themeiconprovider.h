#ifndef _THEME_ICON_PROVIDER_H_
#define _THEME_ICON_PROVIDER_H_

#include <QDeclarativeImageProvider>

class ThemeIconProvider : public QDeclarativeImageProvider
{
public:
    ThemeIconProvider();

    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);
};

#endif
