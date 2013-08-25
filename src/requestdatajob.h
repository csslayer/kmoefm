#ifndef _REQUEST_DATA_JOB_H_
#define _REQUEST_DATA_JOB_H_
#include <QObject>
#include <QBuffer>
#include <QUrl>
#include <QtOAuth/QtOAuth>
#include <QNetworkRequest>

class RequestDataJob : public QObject
{
    Q_OBJECT

public:
    RequestDataJob(const QUrl& url, QOAuth::HttpMethod method, QOAuth::ParamMap paramMap = QOAuth::ParamMap(), QObject* parent = 0);
    ~RequestDataJob();

public Q_SLOTS:
    void start();
    void replyFinished();
    void readyRead();
    const QByteArray& data();

Q_SIGNALS:
    void finished(bool success);

private:
    QBuffer m_buffer;
    QByteArray m_inlineString;
    QNetworkRequest m_request;
    QOAuth::HttpMethod m_method;
};

#endif
