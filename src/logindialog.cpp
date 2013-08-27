#include "logindialog.h"
#include "accesstokenjob.h"
#include "requesttokenjob.h"
#include "networkaccessmanager.h"
#include "app.h"
#include <KMessageBox>
#include <KLocalizedString>
#include <QtOAuth/QtOAuth>
#include <QWebView>
#include <QVBoxLayout>
#include <QDebug>

LoginDialog::LoginDialog(QWidget* parent): QDialog(parent)
{
    RequestTokenJob* auth = new RequestTokenJob;
    QThread* thread = new QThread;

    auth->moveToThread(thread);
    setWindowTitle(i18n("Login"));

    connect(thread, SIGNAL(started()), auth, SLOT(start()));
    connect(auth, SIGNAL(finished(bool,QByteArray,QByteArray)), thread, SLOT(quit()));
    connect(auth, SIGNAL(finished(bool,QByteArray,QByteArray)), auth, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(auth, SIGNAL(finished(bool,QByteArray,QByteArray)), SLOT(requestTokenFinished(bool,QByteArray,QByteArray)));

    thread->start();

    QVBoxLayout* l = new QVBoxLayout(this);
    setLayout(l);

    webView = new QWebView(this);
    webView->settings()->globalSettings()->setAttribute (QWebSettings::LocalStorageEnabled, true);
    webView->settings()->globalSettings()->setAttribute (QWebSettings::OfflineStorageDatabaseEnabled, true);
    webView->settings()->globalSettings()->setAttribute (QWebSettings::JavascriptCanOpenWindows, true);
    webView->settings()->globalSettings()->setAttribute (QWebSettings::JavascriptCanAccessClipboard, true);
    webView->settings()->globalSettings()->setAttribute (QWebSettings::JavascriptEnabled, true);
    webView->page()->setNetworkAccessManager(moeApp->networkAccessManager());

    l->addWidget(webView);

}

void LoginDialog::requestTokenFinished(bool finished, const QByteArray& requestToken , const QByteArray& tokenSecret )
{
    if (finished) {
        QUrl url("http://api.moefou.org/oauth/authorize");
        url.addQueryItem ("oauth_token", requestToken);
        url.addQueryItem ("oauth_callback", "qmoefm://custom");
        webView->load(url);
        webView->show();

        oauthToken = requestToken;
        oauthTokenSecret = tokenSecret;
        connect(webView, SIGNAL(urlChanged(QUrl)), SLOT(urlChanged(QUrl)));
    } else {
        KMessageBox::error(this, i18n("Fetching request token failed, please check your network."));
        reject();
    }
}

LoginDialog::~LoginDialog()
{

}

void LoginDialog::urlChanged(const QUrl& url)
{
    if (url.scheme() == "qmoefm" && url.hasQueryItem("oauth_verifier")) {
        QString verifer = url.queryItemValue("oauth_verifier");
        QOAuth::ParamMap params;
        params.insert(QByteArray("oauth_verifier"), verifer.toLatin1());
        AccessTokenJob* auth = new AccessTokenJob(oauthToken, oauthTokenSecret, params);
        QThread* thread = new QThread;

        auth->moveToThread(thread);

        connect(thread, SIGNAL(started()), auth, SLOT(start()));
        connect(auth, SIGNAL(finished(bool,QByteArray,QByteArray)), thread, SLOT(quit()));
        connect(auth, SIGNAL(finished(bool,QByteArray,QByteArray)), auth, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        connect(auth, SIGNAL(finished(bool,QByteArray,QByteArray)), SLOT(accessTokenFinished(bool,QByteArray,QByteArray)));

        thread->start();

    }
}

void LoginDialog::accessTokenFinished(bool finished, const QByteArray& token, const QByteArray& secret)
{
    if (finished) {
        m_token = token;
        m_secret = secret;
        accept();
    } else {
        KMessageBox::error(this, i18n("Fetching access token failed, please check your network."));
        reject();
    }
}

const QByteArray& LoginDialog::token() const
{
    return m_token;
}

const QByteArray& LoginDialog::secret() const
{
    return m_secret;
}
