#include "misc.h"
#include "requesttokenjob.h"
#include <QtOAuth/QtOAuth>

#include <QDebug>


RequestTokenJob::RequestTokenJob(QObject* parent): QObject(parent)
{
}

RequestTokenJob::~RequestTokenJob()
{

}

void RequestTokenJob::start()
{
    QOAuth::Interface oauth(this);
    oauth.setConsumerKey(CONSUMER_KEY);
    oauth.setConsumerSecret(CONSUMER_SECRET);

    QOAuth::ParamMap map = oauth.requestToken(REQUEST_TOKEN_URL, QOAuth::POST, QOAuth::HMAC_SHA1);

    QByteArray oauthToken, oauthTokenSecret;
    bool success = false;
    if (oauth.error() == 200) {
        oauthToken = map.value (QOAuth::tokenParameterName());
        oauthTokenSecret = map.value (QOAuth::tokenSecretParameterName());
        success = true;
    }

    emit finished(success, oauthToken, oauthTokenSecret);
}
