#include "controller.h"
#include "requestdatajob.h"
#include "app.h"
#include <AudioOutput>
#include <MediaObject>
#include <QDeclarativePropertyMap>
#include <QPalette>
#include <qjson/parser.h>

Controller::Controller(QObject* parent): QObject(parent)
    ,m_parser(new QJson::Parser)
    ,m_mediaObject(new Phonon::MediaObject)
    ,m_output(new Phonon::AudioOutput(Phonon::MusicCategory))
    ,m_path(Phonon::createPath(m_mediaObject, m_output))
    ,m_current(new QDeclarativePropertyMap(this))
{
    setCurrentMusic(Music());
    installEventFilter(moeApp);
    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SIGNAL(stateChanged()));
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
    RequestDataJob* job = new RequestDataJob(QUrl("http://moe.fm/listen/playlist"), QOAuth::GET);

    // connect(job, SIGNAL(finished(bool)), moeApp, SLOT(debugJob(bool)));
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
        if (!itemMap.contains("wiki_url") || !itemMap["wiki_url"].canConvert<QString>()) {
            continue;
        }
        if (!itemMap.contains("sub_url") || !itemMap["sub_url"].canConvert<QString>()) {
            continue;
        }

        QString cover;
        if (itemMap.contains("cover") && itemMap["cover"].canConvert<QVariantMap>()) {
            QMap< QString, QVariant > coverMap = itemMap["cover"].toMap();
            if (coverMap.contains("square") && coverMap["square"].canConvert<QString>()) {
                cover = coverMap["square"].toString();
            }
        }
        // qDebug() << itemMap["stream_time"].toString() << itemMap["title"].toString();

        Music m;
        m.title = itemMap["title"].toString();
        m.artist = itemMap["artist"].toString();
        m.album = itemMap["wiki_title"].toString();
        m.albumUrl = itemMap["wiki_url"].toString();
        m.songUrl = itemMap["sub_url"].toString();
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

void Controller::like()
{
    QOAuth::ParamMap params;
    params.insert("fav_obj_type", "song");
    params.insert("fav_obj_id", (*m_current)["id"].toString().toLatin1());
    params.insert("fav_type", "1");
    params.insert("save_status", "1");
    RequestDataJob* job = new RequestDataJob(
        QUrl("http://api.moefou.org/fav/add.json"),
        QOAuth::GET,
        params
    );
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
    disconnect(m_mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)));
    m_mediaObject->setCurrentSource(m.url);
    m_mediaObject->play();
    connect(m_mediaObject, SIGNAL(finished()), this, SLOT(playerFinished()));

    m_current->insert("id", m.id);
    m_current->insert("title", m.title);
    m_current->insert("songUrl", m.songUrl);
    m_current->insert("artist", m.artist);
    m_current->insert("album", m.album);
    m_current->insert("albumUrl", m.albumUrl);
    m_current->insert("cover", m.cover);

    emit infoChanged();
}

void Controller::playerFinished()
{
    if (m_mediaObject->state() == Phonon::ErrorState) {
        return;
    }

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
