
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
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this, &IPGeoLocation::onProcessTerminated);
}


void IPGeoLocation::doRequest()
{
    QString url = QStringLiteral("https://ipinfo.io/json");
    QString agent = QStringLiteral("Mozilla/5.0");

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, agent);

    connect(&manager, &QNetworkAccessManager::finished, this, &IPGeoLocation::onNetworkReply);
    manager.get(request);
}    

void IPGeoLocation::onNetworkReply(QNetworkReply *reply)
{
    IPInfo _ipinfo;
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (jsonDoc.isObject())
        {
            QJsonObject jsonObj = jsonDoc.object();
            _ipinfo.ip = jsonObj.value(u"ip").toString();
            _ipinfo.city = jsonObj.value(u"city").toString();
            _ipinfo.region = jsonObj.value(u"region").toString();
            _ipinfo.country = jsonObj.value(u"country").toString();
            LogMsg(tr("IPGeoLocation::onReplyFinished() IP %1 City %2 Region %3 Country %4").arg(jsonObj.value(u"ip").toString(),jsonObj.value(u"city").toString(),jsonObj.value(u"region").toString(),jsonObj.value(u"country").toString()));
            QString result=generateIPInfoHTML(_ipinfo);
            emit sigRequestCompleted(result);
        }
    }
    else
    {
        LogMsg(tr("HTTP ERROR %1").arg(reply->errorString()));
        emit sigRequestFailed(reply->errorString());
    }

    reply->deleteLater();
}

void IPGeoLocation::onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus)
{
    LogMsg(tr("IPGeoLocation::onProcessTerminated() - Process finished with exit code: %1").arg(exitCode));
    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        LogMsg(tr("IPGeoLocation::onProcessTerminated() - Process crashed or exited with error."));
        emit sigRequestFailed(QStringLiteral("Speed test failed."));
        return;
    }

}

QString IPGeoLocation::generateIPInfoHTML(const IPInfo &info) const
{
    return u"<html><head/><body><p>"
           u"<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">"
           u"<tr><th>Field</th><th>Value</th></tr>"
           u"<tr><td><b>IP</b></td><td>%1</td></tr>"
           u"<tr><td><b>City</b></td><td>%2</td></tr>"
           u"<tr><td><b>Region</b></td><td>%3</td></tr>"
           u"<tr><td><b>Country</b></td><td>%4</td></tr>"
           u"</table>"
           u"</body></html>"_s
        .arg(info.ip, info.city, info.region, info.country);
}


