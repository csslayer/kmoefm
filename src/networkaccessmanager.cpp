#include <QNetworkRequest>

#include "networkaccessmanager.h"
#include "reply.h"

NetworkAccessManager::NetworkAccessManager (QObject* parent) : QNetworkAccessManager (parent)
{
}

QNetworkReply* NetworkAccessManager::createRequest (QNetworkAccessManager::Operation op, const QNetworkRequest& request, QIODevice* outgoingData)
{
    if ((request.url().scheme() == "qmoefm") && op == QNetworkAccessManager::GetOperation)
        return new Reply (request.url(), this);

    return QNetworkAccessManager::createRequest (op, request, outgoingData);
}
