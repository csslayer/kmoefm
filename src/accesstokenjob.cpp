#include "accesstokenjob.h"
#include "misc.h"
#include <QtOAuth/QtOAuth>

#include <QDebug>

AccessTokenJob::AccessTokenJob(const QByteArray& token, const QByteArray& secret, const QOAuth::ParamMap& params, QObject* parent): QObject(parent)
    ,m_token(token)
    ,m_secret(secret)
    ,m_params(params)
{
}

AccessTokenJob::~AccessTokenJob()
{

}

void AccessTokenJob::start()
{
    QOAuth::Interface oauth(this);
    oauth.setConsumerKey(CONSUMER_KEY);
    oauth.setConsumerSecret(CONSUMER_SECRET);

    qDebug() << m_token << m_secret << m_params;
    QOAuth::ParamMap map = oauth.accessToken (ACCESS_TOKEN_URL, QOAuth::POST, m_token, m_secret, QOAuth::HMAC_SHA1, m_params);

    bool success = false;
    QByteArray token, secret;
    if (oauth.error() == QOAuth::NoError) {
        success = true;
        token = map.value (QOAuth::tokenParameterName());
        secret = map.value (QOAuth::tokenSecretParameterName());
    }

    emit finished(success, token, secret);
}
