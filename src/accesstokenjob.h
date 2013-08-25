#ifndef _ACCESS_TOKEN_JOB_H_
#define _ACCESS_TOKEN_JOB_H_
#include <QObject>
#include <QtOAuth/QtOAuth>

class AccessTokenJob : public QObject
{
    Q_OBJECT

public:
    AccessTokenJob(const QByteArray& token, const QByteArray& secret, const QOAuth::ParamMap& params, QObject* parent = 0);
    ~AccessTokenJob();

public Q_SLOTS:
    void start();

Q_SIGNALS:
    void finished(bool success, const QByteArray& requestToken, const QByteArray& tokenSecret);

private:
    QByteArray m_token;
    QByteArray m_secret;
    QOAuth::ParamMap m_params;
};

#endif
