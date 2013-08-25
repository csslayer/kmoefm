#ifndef _APP_H_
#define _APP_H_

#include <KUniqueApplication>
#include <QtOAuth/QtOAuth>
#include <QBuffer>
#include <QSettings>
#include "controller.h"

class KStatusNotifierItem;
class MainWindow;
namespace QJson {
class Parser;
}

class NetworkAccessManager;
class LoginDialog;
class App : public KUniqueApplication
{
Q_OBJECT
public:
    App();
    virtual ~App();

    NetworkAccessManager* networkAccessManager() const;

    QByteArray inlineString(const QOAuth::ParamMap& paramMap, QOAuth::ParsingMode mode);
    QByteArray createParametersString(const QUrl& url, QOAuth::HttpMethod method, QOAuth::ParamMap params, QOAuth::ParsingMode mode = QOAuth::ParseForHeaderArguments);

    Controller* controller();
public slots:
    void finished(bool success);
    void firstRunCheck(bool);
    void debugJob(bool);
    void checkCredential();
    void login();
    void relogin();
    void updateInfo();
signals:
    void checkCredentialFinished(bool);

private:
    void readSettings();
    void writeSettings();

    NetworkAccessManager* m_networkAccessManager;
    QOAuth::Interface* m_oauth;
    QByteArray m_token;
    QByteArray m_secret;
    QJson::Parser* m_parser;
    MainWindow* m_mainWindow;
    Controller* m_controller;
    KStatusNotifierItem* m_tray;
    QString m_title;
};

#if defined(moeApp)
#undef moeApp
#endif
#define moeApp (static_cast<App *>(QCoreApplication::instance()))

#endif // _APP_H_
