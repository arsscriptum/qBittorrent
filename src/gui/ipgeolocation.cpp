
#include "ipgeolocation.h"

#include <QProcess>
#include <QClipboard>
#include <QApplication>
#include "ipgeolocation.h"
#include <QProcess>
#include "base/global.h"
#include "base/path.h"
#include "base/unicodestrings.h"
#include "base/utils/io.h"
#include "base/utils/misc.h"
#include "base/version.h"
#include "ui_networkdialog.h"
#include "uithememanager.h"
#include "utils.h"
#include "base/global.h"
#include "base/logger.h"


IPGeoLocation::IPGeoLocation(QObject *parent) 
    : QObject(parent), m_process(new QProcess(this))
{
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &IPGeoLocation::onProcessFinished);
}


void IPGeoLocation::doRequest()
{
    QString url = QStringLiteral("https://ipinfo.io/json");
    QString agent = QStringLiteral("Mozilla/5.0");

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, agent);

    connect(&manager, &QNetworkAccessManager::finished, this, &IPGeoLocation::onReplyFinished);
    manager.get(request);
}    

void IPGeoLocation::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        if (jsonDoc.isObject())
        {
            QJsonObject jsonObj = jsonDoc.object();
            LogMsg(tr("IPGeoLocation::onReplyFinished() IP %1 City %2 Region %3 Country %4").arg(jsonObj.value(u"ip").toString(),jsonObj.value(u"city").toString(),jsonObj.value(u"region").toString(),jsonObj.value(u"country").toString()));
            
        }
    }
    else
    {
         qWarning() << "HTTP Error:" << reply->errorString();
    }
    reply->deleteLater();
}

void IPGeoLocation::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    LogMsg(tr("IPGeoLocation::onProcessFinished() - Process finished with exit code: %1").arg(exitCode));

    // Read the
}