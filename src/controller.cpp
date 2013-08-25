#include "controller.h"
#include "requestdatajob.h"
#include "app.h"
#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <QDeclarativePropertyMap>
#include <QPalette>
#include <qjson/parser.h>
#include <KDebug>
#include <iostream>

Controller::Controller(QObject* parent): QObject(parent)
    ,m_parser(new QJson::Parser)
    ,m_mediaObject(new Phonon::MediaObject)
    ,m_output(new Phonon::AudioOutput(Phonon::MusicCategory))
    ,m_path(Phonon::createPath(m_mediaObject, m_output))
    ,m_current(new QDeclarativePropertyMap(this))
{
    setCurrentMusic(Music());
    installEventFilter(moeApp);
    m_mediaObject->setTickInterval(900);
    connect(m_mediaObject, SIGNAL(tick(qint64)), this, SIGNAL(timeChanged(qint64)));
    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SIGNAL(stateChanged()));
    connect(m_mediaObject, SIGNAL(finished()), this, SLOT(playerFinished()));
}

Controller::~Controller()
{
    delete m_parser;
    delete m_output;
    delete m_mediaObject;
}

bool Controller::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == qApp) {
        if (event->type() == QEvent::ApplicationPaletteChange) {
            emit colorChanged();
        }
    }
    return QObject::eventFilter(watched, event);
}


void Controller::loadMusic()
{
    QOAuth::ParamMap params;
    RequestDataJob* job = new RequestDataJob(QUrl("http://moe.fm/listen/playlist"), QOAuth::GET, params);

    connect(job, SIGNAL(finished(bool)), moeApp, SLOT(debugJob(bool)));
    connect(job, SIGNAL(finished(bool)), job, SLOT(deleteLater()));
    connect(job, SIGNAL(finished(bool)), SLOT(loadMusicFinished(bool)));

    job->start();
}

void Controller::loadMusicFinished(bool success)
{
    if (!success) {
        return;
    }

    RequestDataJob* job = static_cast<RequestDataJob*>(sender());
    QVariant result = m_parser->parse(job->data());
    if (!result.canConvert<QVariantMap>()) {
        return;
    }

    QMap< QString, QVariant > map = result.toMap();
    if (!map.contains("playlist")) {
        return;
    }

    QVariant v = map["playlist"];
    if (!v.canConvert<QVariantList>()) {
        return;
    }

    QList< QVariant > playList = v.toList();
    foreach(const QVariant& item, playList) {
        if (!item.canConvert<QVariantMap>()) {
            continue;
        }

        QVariantMap itemMap = item.toMap();
        if (!itemMap.contains("title") || !itemMap["title"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("sub_id") || !itemMap["sub_id"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("url") || !itemMap["url"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("artist") || !itemMap["artist"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("wiki_title") || !itemMap["wiki_title"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("wiki_id") || !itemMap["wiki_id"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("wiki_url") || !itemMap["wiki_url"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("sub_url") || !itemMap["sub_url"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("stream_time") || !itemMap["stream_time"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("stream_length") || !itemMap["stream_length"].canConvert<QString>()) {
            continue;
        }

        QString cover;
        if (itemMap.contains("cover") && itemMap["cover"].canConvert<QVariantMap>()) {
            QMap< QString, QVariant > coverMap = itemMap["cover"].toMap();
            if (coverMap.contains("square") && coverMap["square"].canConvert<QString>()) {
                cover = coverMap["square"].toString();
            }
        }

        QString favId;
        if (itemMap.contains("fav_sub") && itemMap["fav_sub"].canConvert<QString>()) {
            favId = itemMap["fav_sub"].toString();
        }

        QString favAlbum;
        if (itemMap.contains("fav_wiki") && itemMap["fav_wiki"].canConvert<QString>()) {
            favId = itemMap["fav_wiki"].toString();
        }
        // qDebug() << itemMap["stream_time"].toString() << itemMap["title"].toString();

        Music m;
        m.favId = favId;
        m.favAlbum = favAlbum;
        m.title = itemMap["title"].toString();
        m.albumId = itemMap["wiki_id"].toString();
        m.artist = itemMap["artist"].toString();
        m.album = itemMap["wiki_title"].toString();
        m.albumUrl = itemMap["wiki_url"].toString();
        m.songUrl = itemMap["sub_url"].toString();
        m.streamTime = itemMap["stream_time"].toString();
        m.streamLength = itemMap["stream_length"].toString().toLongLong();
        m.cover = cover;
        m.id = itemMap["sub_id"].toString();
        m.url = QUrl(itemMap["url"].toString());
        m_songs << m;
    }

    if (!m_songs.isEmpty()) {
        playNext();
    }
}

void Controller::playNext() {
    if (m_songs.isEmpty()) {
        loadMusic();
        return;
    }

    Music music = m_songs.first();
    m_songs.removeFirst();
    setCurrentMusic(music);
}

void Controller::playPause()
{
    if (m_mediaObject->state() == Phonon::PlayingState) {
        m_mediaObject->pause();
    } else if (m_mediaObject->state() == Phonon::PausedState) {
        m_mediaObject->play();
    }
}

void Controller::like(bool isAlbum)
{
    const char* checkField = isAlbum ? "favAlbum" : "favId";
    const char* typeField = isAlbum ? "music" : "song";
    const char* objIdField = isAlbum ? "albumId" : "id";
    RequestDataJob* job;
    if ((*m_current)[checkField].toString().length() > 0) {
        QOAuth::ParamMap params;
        params.insert("fav_obj_type", typeField);
        params.insert("fav_obj_id", (*m_current)[objIdField].toString().toLatin1());
        job = new RequestDataJob(
            QUrl("http://api.moefou.org/fav/delete.json"),
            QOAuth::GET,
            params
        );
        m_current->insert(checkField, "");
        connect(job, SIGNAL(finished(bool)), moeApp, SLOT(debugJob(bool)));
        emit infoChanged();
    } else {
        QOAuth::ParamMap params;
        params.insert("fav_obj_type", typeField);
        params.insert("fav_obj_id", (*m_current)[objIdField].toString().toLatin1());
        params.insert("fav_type", "1");
        params.insert("save_status", "1");
        job = new RequestDataJob(
            QUrl("http://api.moefou.org/fav/add.json"),
            QOAuth::GET,
            params
        );
        connect(job, SIGNAL(finished(bool)), this, SLOT(favFinshed(bool)));
        job->setProperty("isAlbum", isAlbum);
    }
    // connect(job, SIGNAL(finished(bool)), moeApp, SLOT(debugJob(bool)));
    connect(job, SIGNAL(finished(bool)), job, SLOT(deleteLater()));
    job->start();
}

QDeclarativePropertyMap* Controller::info() const
{
    return m_current;
}

void Controller::setCurrentMusic(const Music& m)
{
    m_mediaObject->clear();
    m_mediaObject->enqueue(m.url);
    m_mediaObject->play();

    m_current->insert("id", m.id);
    m_current->insert("albumId", m.albumId);
    m_current->insert("title", m.title);
    m_current->insert("songUrl", m.songUrl);
    m_current->insert("artist", m.artist);
    m_current->insert("album", m.album);
    m_current->insert("albumUrl", m.albumUrl);
    m_current->insert("cover", m.cover);
    m_current->insert("streamTime", m.streamTime);
    m_current->insert("streamLength", m.streamLength);
    m_current->insert("favId", m.favId);
    m_current->insert("favAlbum", m.favAlbum);

    emit infoChanged();
}

void Controller::playerFinished()
{
    if (m_mediaObject->state() != Phonon::ErrorState) {
        QOAuth::ParamMap params;
        params.insert("log_obj_type", "sub");
        params.insert("log_type", "listen");
        params.insert("obj_type", "song");
        params.insert("api", "json");
        params.insert("obj_id", (*m_current)["id"].toString().toLatin1());
        RequestDataJob* job = new RequestDataJob(
            QUrl("http://moe.fm/ajax/log"),
            QOAuth::GET,
            params
        );
        // connect(job, SIGNAL(finished(bool)), moeApp, SLOT(debugJob(bool)));
        connect(job, SIGNAL(finished(bool)), job, SLOT(deleteLater()));
        job->start();
    }

    playNext();
}

QColor Controller::textColor() const
{
    return qApp->palette().color(QPalette::Active, QPalette::Text);
}

QColor Controller::linkColor() const
{
    return qApp->palette().color(QPalette::Active, QPalette::Link);
}

bool Controller::isPaused() const
{
    return m_mediaObject->state() == Phonon::PausedState;
}

void Controller::favFinshed(bool success)
{
    RequestDataJob* job = static_cast<RequestDataJob*>(sender());
    if (!success) {
        return;
    }
    bool isAlbum = job->property("isAlbum").toBool();
    QVariant result = m_parser->parse(job->data());
    if (!result.canConvert<QVariantMap>()) {
        return;
    }

    QMap< QString, QVariant > itemMap = result.toMap();
    if (!itemMap.contains("response") || !itemMap["response"].canConvert<QVariantMap>()) {
        return;
    }
    itemMap = itemMap["response"].toMap();

    if (!itemMap.contains("fav") || !itemMap["fav"].canConvert<QVariantMap>()) {
        return;
    }
    itemMap = itemMap["fav"].toMap();

    if (!itemMap.contains("fav_id") || !itemMap["fav_id"].canConvert<qulonglong>()) {
        return;
    }

    m_current->insert(isAlbum ? "favAlbum" : "favId", "1");
    emit infoChanged();
    emit favoriteAdded();
}

bool Controller::isPlaying() const
{
    return m_mediaObject->state() == Phonon::PlayingState;
}


qint64 Controller::time() const
{
    return m_mediaObject->currentTime() / 1000;
}
