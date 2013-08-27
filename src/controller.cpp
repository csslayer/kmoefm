#include "controller.h"
#include "requestdatajob.h"
#include "app.h"
#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <QDeclarativePropertyMap>
#include <QTextDocument>
#include <QPalette>
#include <qjson/parser.h>
#include <KDebug>
#include <QFontMetrics>
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
        } else if (event->type() == QEvent::ApplicationFontChange) {
            emit fontChanged();
        }
    }
    return QObject::eventFilter(watched, event);
}


void Controller::loadMusic()
{
    QOAuth::ParamMap params;
    params.insert("api", "json");
    RequestDataJob* job = new RequestDataJob(QUrl("http://moe.fm/listen/playlist"), QOAuth::GET, params);

    // connect(job, SIGNAL(finished(bool)), moeApp, SLOT(debugJob(bool)));
    connect(job, SIGNAL(finished(bool)), job, SLOT(deleteLater()));
    connect(job, SIGNAL(finished(bool)), SLOT(loadMusicFinished(bool)));

    job->start();
}

template<class T>
bool checkValue(const QVariantMap& map, const QString& key)
{
    return map.contains(key) && map[key].canConvert<T>();
}

QString normalizeString(const QString& s)
{
    QTextDocument d;
    d.setHtml(s);
    return d.toPlainText();
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

    QVariantMap map = result.toMap();
    if (!checkValue<QVariantMap>(map, "response")) {
        return;
    }

    map = map["response"].toMap();
    if (!checkValue<QVariantList>(map, "playlist")) {
        return;
    }

    QList< QVariant > playList = map["playlist"].toList();
    foreach(const QVariant& item, playList) {
        if (!item.canConvert<QVariantMap>()) {
            continue;
        }

        QVariantMap itemMap = item.toMap();
        QStringList valueToCheck;
        valueToCheck << "sub_title" << "sub_id" << "url" << "artist"
                     << "wiki_title" << "wiki_id" << "wiki_url"
                     << "sub_url" << "stream_time" << "stream_length";
        bool flag = false;
        foreach(const QString& key, valueToCheck) {
            if (!checkValue<QString>(itemMap, key)) {
                flag = true;
                break;
            }
        }
        if (flag) {
            continue;
        }

        QString cover;
        if (checkValue<QVariantMap>(itemMap, "cover")) {
            QVariantMap coverMap = itemMap["cover"].toMap();
            if (checkValue<QString>(coverMap, "square")) {
                cover = coverMap["square"].toString();
            }
        }

        qulonglong favType = 0;
        if (checkValue<QVariantMap>(itemMap, "fav_sub")) {
            QVariantMap favMap = itemMap["fav_sub"].toMap();
            if (checkValue<qulonglong>(favMap, "fav_type")) {
                favType = favMap["fav_type"].toULongLong();
            }
        }

        qulonglong favAlbumType = 0;
        if (checkValue<QVariantMap>(itemMap, "fav_wiki")) {
            QVariantMap favMap = itemMap["fav_wiki"].toMap();
            if (checkValue<qulonglong>(favMap, "fav_type")) {
                favAlbumType = favMap["fav_type"].toULongLong();
            }
        }
        // qDebug() << itemMap["stream_time"].toString() << itemMap["title"].toString();

        Music m;
        m.favType = favType;
        m.favAlbumType = favAlbumType;
        m.title = normalizeString(itemMap["sub_title"].toString());
        m.albumId = itemMap["wiki_id"].toString();
        m.artist = normalizeString(itemMap["artist"].toString());
        m.album = normalizeString(itemMap["wiki_title"].toString());
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

void Controller::like(bool isAlbum, int type)
{
    const char* checkField = isAlbum ? "favAlbumType" : "favType";
    const char* typeField = isAlbum ? "music" : "song";
    const char* objIdField = isAlbum ? "albumId" : "id";
    RequestDataJob* job;
    if ((*m_current)[checkField] == type) {
        QOAuth::ParamMap params;
        params.insert("fav_obj_type", typeField);
        params.insert("fav_obj_id", (*m_current)[objIdField].toString().toLatin1());
        job = new RequestDataJob(
            QUrl("http://api.moefou.org/fav/delete.json"),
            QOAuth::GET,
            params
        );
        connect(job, SIGNAL(finished(bool)), this, SLOT(favDelFinshed(bool)));
        // connect(job, SIGNAL(finished(bool)), moeApp, SLOT(debugJob(bool)));
        emit infoChanged();
    } else {
        QOAuth::ParamMap params;
        params.insert("fav_obj_type", typeField);
        params.insert("fav_obj_id", (*m_current)[objIdField].toString().toLatin1());
        params.insert("fav_type", QByteArray::number(type)) ;
        params.insert("save_status", "1");
        job = new RequestDataJob(
            QUrl("http://api.moefou.org/fav/add.json"),
            QOAuth::GET,
            params
        );
        connect(job, SIGNAL(finished(bool)), this, SLOT(favFinished(bool)));
    }
    job->setProperty("isAlbum", isAlbum);
    job->setProperty("type", type);
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
    if (!m.url.isEmpty()) {
        m_mediaObject->enqueue(m.url);
        m_mediaObject->play();
    }

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
    m_current->insert("favType", m.favType);
    m_current->insert("favAlbumType", m.favAlbumType);

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

QColor Controller::windowColor() const
{
    return qApp->palette().color(QPalette::Active, QPalette::Window);
}

bool Controller::isPaused() const
{
    return m_mediaObject->state() == Phonon::PausedState;
}

void Controller::favFinished(bool success)
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

    QVariantMap itemMap = result.toMap();
    if (!checkValue<QVariantMap>(itemMap, "response")) {
        return;
    }
    itemMap = itemMap["response"].toMap();

    if (!checkValue<QVariantMap>(itemMap, "information")) {
        return;
    }
    itemMap = itemMap["information"].toMap();

    if (!checkValue<bool>(itemMap, "has_error")) {
        return;
    }
    if (itemMap["has_error"].toBool()) {
        return;
    }

    qulonglong type = job->property("type").toULongLong();
    m_current->insert(isAlbum ? "favAlbumType" : "favType", type);
    emit infoChanged();
    emit favoriteChanged(type);
}

void Controller::favDelFinshed(bool success)
{
    RequestDataJob* job = static_cast<RequestDataJob*>(sender());
    if (!success) {
        return;
    }
    bool isAlbum = job->property("isAlbum").toBool();
    m_current->insert(isAlbum ? "favAlbumType" : "favType", 0);
    emit infoChanged();

}

bool Controller::isPlaying() const
{
    return m_mediaObject->state() == Phonon::PlayingState;
}

qint64 Controller::time() const
{
    return m_mediaObject->currentTime() / 1000;
}

int Controller::fontSize() const
{
    return moeApp->font().pointSize();
}
