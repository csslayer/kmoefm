#ifndef _REQUEST_TOKEN_JOB_H_
#define _REQUEST_TOKEN_JOB_H_
#include <QObject>

class RequestTokenJob : public QObject
{
    Q_OBJECT

public:
    RequestTokenJob(QObject* parent = 0);
    ~RequestTokenJob();

public Q_SLOTS:
    void start();

Q_SIGNALS:
    void finished(bool success, const QByteArray& requestToken, const QByteArray& tokenSecret);
};

#endif
