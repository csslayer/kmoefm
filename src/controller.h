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
public:
    explicit Controller(QObject* parent = 0);
    virtual ~Controller();
public slots:
    void playNext();
    void playPause();
    void like();
    void loadMusicFinished(bool);
    void loadMusic();
    void playerFinished();
    virtual bool eventFilter(QObject* , QEvent* );
    void favFinshed(bool success);
    void playerStateChanged(Phonon::State,Phonon::State);

public:
    QDeclarativePropertyMap* info() const;
    QColor linkColor() const;
    QColor textColor() const;
    bool isPaused() const;

signals:
    void infoChanged();
    void colorChanged();
    void stateChanged();
    void favoriteAdded();

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
