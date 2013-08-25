#include "requestdatajob.h"
#include "app.h"
#include "networkaccessmanager.h"
#include <QtOAuth/QtOAuth>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

RequestDataJob::RequestDataJob(const QUrl& url, QOAuth::HttpMethod method, QOAuth::ParamMap paramMap, QObject* parent) : QObject(parent)
    ,m_inlineString(moeApp->inlineString (paramMap, QOAuth::ParseForRequestContent))
    ,m_request(url)
    ,m_method(method)
{
    if (method == QOAuth::POST) {
        m_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        for (QOAuth::ParamMap::iterator it = paramMap.begin(); it != paramMap.end(); it++) {
            it.value() = QUrl::toPercentEncoding(QString::fromUtf8(it.value()));
        }
        m_request.setRawHeader("Authorization", moeApp->createParametersString(m_request.url(), method, paramMap, QOAuth::ParseForRequestContent));
    } else if (method == QOAuth::GET) {
        QUrl newUrl(url.toString().append(moeApp->createParametersString(m_request.url(), method, paramMap, QOAuth::ParseForInlineQuery)));
        m_request.setUrl(newUrl);
        qDebug() << m_request.url();
    }

}

RequestDataJob::~RequestDataJob()
{

}

void RequestDataJob::start()
{
    m_buffer.close();
    m_buffer.buffer().clear();
    m_buffer.open(QIODevice::WriteOnly);
    QNetworkReply* reply = 0;
    if (m_method == QOAuth::POST) {
        reply = moeApp->networkAccessManager()->post (m_request, m_inlineString);
    } else if (m_method == QOAuth::GET) {
        reply = moeApp->networkAccessManager()->get (m_request);
    }

    if (reply) {
        connect(reply, SIGNAL(readyRead()), SLOT(readyRead()));
        connect(reply, SIGNAL(finished()), SLOT(replyFinished()));
    } else {
        emit finished(false);
    }
}

void RequestDataJob::readyRead()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    m_buffer.write(reply->readAll());
}

void RequestDataJob::replyFinished()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    bool success = false;
    if (reply->error() == QNetworkReply::NoError) {
        success = true;
        m_buffer.seek (0);
        m_buffer.close();
    }

    emit finished(success);
}

const QByteArray& RequestDataJob::data()
{
    return m_buffer.data();
}
