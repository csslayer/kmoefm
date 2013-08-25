#ifndef _LOGINDIALOG_H_
#define _LOGINDIALOG_H_

#include <QDialog>
#include <QUrl>

class QWebView;

class LoginDialog : public QDialog
{
    Q_OBJECT
    QByteArray oauthToken;
    QByteArray oauthTokenSecret;
    QWebView* webView;
    QByteArray m_secret;
    QByteArray m_token;
public:
    explicit LoginDialog(QWidget* parent = 0);
    virtual ~LoginDialog();

    const QByteArray& token() const;
    const QByteArray& secret() const;
public slots:
    void requestTokenFinished(bool finished, const QByteArray& requestToken , const QByteArray& tokenSecret);
    void accessTokenFinished(bool finished, const QByteArray& requestToken , const QByteArray& tokenSecret);
    void urlChanged(const QUrl& url);
};

#endif // _LOGINDIALOG_H_
