#include "app.h"
#include "misc.h"
#include "logindialog.h"
#include "networkaccessmanager.h"
#include "requestdatajob.h"
#include "mainwindow.h"

#include <KStatusNotifierItem>
#include <KConfigGroup>
#include <KDebug>
#include <KActionCollection>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QtOAuth/QtOAuth>
#include <QSettings>
#include <QTimer>
#include <qjson/parser.h>
#include <qdeclarative.h>
#include <KLocalizedString>
#include <KAction>
#include <KMenu>

App::App() : KUniqueApplication()
    ,m_networkAccessManager(new NetworkAccessManager(this))
    ,m_oauth(new QOAuth::Interface(this))
    ,m_parser(new QJson::Parser)
    ,m_mainWindow(0)
    ,m_controller(0)
    ,m_tray(0)
{
    qmlRegisterType<QDeclarativePropertyMap>();

    m_controller = new Controller;
    m_mainWindow = new MainWindow;
    m_tray = new KStatusNotifierItem(this);
    m_tray->setIconByName("kmoefm");
    m_tray->setTitle(i18n("Moe FM"));
    m_tray->setToolTipIconByName("kmoefm");
    m_tray->setToolTipTitle(i18n("KMoeFM"));
    m_tray->setToolTipSubTitle(i18n("Moe FM"));
    KAction* action;
    action = m_tray->actionCollection()->addAction(QLatin1String("relogin"), this, SLOT(relogin()));
    action->setText(i18n("Relogin"));
    m_tray->contextMenu()->addAction(action);
    m_tray->setAssociatedWidget(m_mainWindow);

    connect(m_controller, SIGNAL(infoChanged()), this, SLOT(updateInfo()));

    m_oauth->setConsumerKey(CONSUMER_KEY);
    m_oauth->setConsumerSecret(CONSUMER_SECRET);

    readSettings();

    connect(this, SIGNAL(checkCredentialFinished(bool)), SLOT(firstRunCheck(bool)));
    QTimer::singleShot(0, this, SLOT(checkCredential()));
}

App::~App()
{
    delete m_controller;
}

NetworkAccessManager* App::networkAccessManager() const
{
    return m_networkAccessManager;
}

void App::updateInfo()
{
    QString title = (*moeApp->controller()->info())["title"].toString();
    if (m_title == title) {
        return;
    }
    m_title = title;
    m_tray->setToolTipSubTitle(title);
    m_tray->showMessage(i18n("Moe FM"), title, "kmoefm", 1000);
    if (title.isEmpty()) {
        title = i18n("Moe FM");
    } else {
        title = QString(i18n("%1 - Moe FM")).arg(title);
    }
    m_mainWindow->setWindowTitle(title);
}


void App::login()
{
    LoginDialog* loginDialog = new LoginDialog(m_mainWindow);
    int result = loginDialog->exec();
    if (result == QDialog::Accepted) {
        // qDebug() << loginDialog->token() << loginDialog->secret();
        m_token = loginDialog->token();
        m_secret = loginDialog->secret();

        writeSettings();

        connect(this, SIGNAL(checkCredentialFinished(bool)), SLOT(firstRunCheck(bool)));
        QTimer::singleShot(0, this, SLOT(checkCredential()));
    } else {
        quit();
    }
    delete loginDialog;
}

void App::relogin()
{
    login();
}

void App::readSettings()
{
    KConfigGroup cg(KGlobal::config(), "Account");
    m_token = cg.readEntry("Token", QByteArray());
    m_secret = cg.readEntry("Secret", QByteArray());
}

void App::checkCredential()
{
    if (m_token.isEmpty() || m_secret.isEmpty()) {
        emit checkCredentialFinished(false);
    }

    RequestDataJob* job = new RequestDataJob(QUrl("http://api.moefou.org/user/detail.json"), QOAuth::GET);
    connect(job, SIGNAL(finished(bool)), SIGNAL(checkCredentialFinished(bool)));
    connect(job, SIGNAL(finished(bool)), job, SLOT(deleteLater()));

    job->start();
}

void App::firstRunCheck(bool success)
{
    this->disconnect(SIGNAL(checkCredentialFinished(bool)));
    if (success) {
        m_mainWindow->show();
        m_tray->setStatus(KStatusNotifierItem::Active);
        m_controller->loadMusic();
    } else {
        login();
    }
}

void App::debugJob(bool success)
{
    RequestDataJob* job = static_cast<RequestDataJob*>(sender());
    QVariant result = m_parser->parse(job->data());
    kDebug() << success << result;
}

void App::writeSettings()
{
    KConfigGroup cg(KGlobal::config(), "Account");
    cg.writeEntry("Token", m_token);
    cg.writeEntry("Secret", m_secret);
    cg.sync();
}


QByteArray App::createParametersString(const QUrl& url, QOAuth::HttpMethod method, QOAuth::ParamMap params, QOAuth::ParsingMode mode)
{
    return m_oauth->createParametersString(QString::fromAscii(url.toEncoded()),
                                           method,
                                           m_token,
                                           m_secret,
                                           QOAuth::HMAC_SHA1,
                                           params,
                                           mode
    );
}

QByteArray App::inlineString(const QOAuth::ParamMap& paramMap, QOAuth::ParsingMode mode)
{
    return m_oauth->inlineParameters(paramMap, mode);
}

void App::finished(bool success)
{
    RequestDataJob* job = static_cast<RequestDataJob*>(sender());
    if (success) {
        QVariant result = m_parser->parse(job->data());
        // qDebug() << result;
    }
}

Controller* App::controller()
{
    return m_controller;
}
