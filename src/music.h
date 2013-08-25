#ifndef _MUSIC_H_
#define _MUSIC_H_
#include <QUrl>

struct Music {
    QUrl url;
    QString title;
    QString artist;
    QString album;
    QString cover;
    QString albumUrl;
    QString songUrl;
    QString id;
    QString streamTime;
    qlonglong streamLength;
    QString favId;
};

#endif
