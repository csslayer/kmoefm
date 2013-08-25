#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include <QObject>
#include <Path>
#include <phononnamespace.h>
#include <QDeclarativePropertyMap>
#include <QColor>
#include "music.h"

class QDeclarativePropertyMap;
namespace Phonon {
class MediaObject;
class AudioOutput;
}

namespace QJson {
class Parser;
}

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativePropertyMap *info READ info NOTIFY infoChanged)
    Q_PROPERTY(QColor linkColor READ linkColor NOTIFY colorChanged)
    Q_PROPERTY(QColor textColor READ textColor NOTIFY colorChanged)
    Q_PROPERTY(bool isPaused READ isPaused NOTIFY stateChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(qint64 time READ time NOTIFY timeChanged)
public:
    explicit Controller(QObject* parent = 0);
    virtual ~Controller();
public slots:
    void playNext();
    void playPause();
    void like(bool isAlbum);
    void loadMusicFinished(bool);
    void loadMusic();
    void playerFinished();
    virtual bool eventFilter(QObject* , QEvent* );
    void favFinshed(bool success);

public:
    QDeclarativePropertyMap* info() const;
    QColor linkColor() const;
    QColor textColor() const;
    bool isPaused() const;
    bool isPlaying() const;
    qint64 time() const;

signals:
    void infoChanged();
    void colorChanged();
    void stateChanged();
    void favoriteAdded();
    void timeChanged(qint64);

private:
    void setCurrentMusic(const Music& m);

private:
    QJson::Parser* m_parser;
    QList<Music> m_songs;
    Phonon::MediaObject* m_mediaObject;
    Phonon::AudioOutput* m_output;
    Phonon::Path m_path;
    QDeclarativePropertyMap* m_current;
};

#endif
