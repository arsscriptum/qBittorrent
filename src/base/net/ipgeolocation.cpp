
#include "ipgeolocation.h"

#include <QProcess>


#include "ipgeolocation.h"
#include <QProcess>
#include "base/global.h"
#include "base/path.h"
#include "base/unicodestrings.h"
#include "base/utils/io.h"
#include "base/utils/misc.h"
#include "base/version.h"
#include "base/global.h"
#include "base/logger.h"



IPGeoLocation::IPGeoLocation(QObject *parent) 
    : QObject(parent), m_process(new QProcess(this))
{
    m_delay = 300;
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),this, &IPGeoLocation::onProcessTerminated);
}


void IPGeoLocation::doRequest(bool force)
{
    static QDateTime lastRequestTime; // Stores the last request timestamp

    
    // Check if at least 5 minutes (300 seconds) have passed since the last request
    if (!force && lastRequestTime.isValid() && lastRequestTime.secsTo(QDateTime::currentDateTime()) < m_delay) {
        LogMsg(tr("IPGeoLocation::doRequest() Skipping request, last request was too recent."));
        return; // Skip the request if it's too soon
    }

    // Update the last request timestamp
    lastRequestTime = QDateTime::currentDateTime();

    // Define request parameters
    QString url = QStringLiteral("https://ipinfo.io/json");
    QString agent = QStringLiteral("Mozilla/5.0");

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, agent);

    connect(&manager, &QNetworkAccessManager::finished, this, &IPGeoLocation::onNetworkReply);

    // Execute the request
    manager.get(request);
    LogMsg(tr("IPGeoLocation::doRequest() Request sent."));
}

void IPGeoLocation::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

        if (jsonDoc.isObject())
        {
            QJsonObject jsonObj = jsonDoc.object();

            // Assign values using setters
            if (jsonObj.contains(u"ip")) {
                QString ip = jsonObj.value(u"ip").toString();
                LogMsg(tr("IPGeoLocation::onNetworkReply() - ip: %1").arg(ip));
                m_ipinfo.setIp(ip);
            }
            if (jsonObj.contains(u"city")) {
                QString city = jsonObj.value(u"city").toString();
                LogMsg(tr("IPGeoLocation::onNetworkReply() - city: %1").arg(city));
                m_ipinfo.setCity(city);
            }
            if (jsonObj.contains(u"region")) {
                QString region = jsonObj.value(u"region").toString();
                LogMsg(tr("IPGeoLocation::onNetworkReply() - region: %1").arg(region));
                m_ipinfo.setRegion(region);
            }
            if (jsonObj.contains(u"country")) {
                QString country = jsonObj.value(u"country").toString();
                LogMsg(tr("IPGeoLocation::onNetworkReply() - country: %1").arg(country));
                m_ipinfo.setCountry(country);
            }
            if (jsonObj.contains(u"hostname")) {
                m_ipinfo.setHostname(jsonObj.value(u"hostname").toString());
            }
            if (jsonObj.contains(u"loc")) {
                m_ipinfo.setLoc(jsonObj.value(u"loc").toString());
            }
            if (jsonObj.contains(u"org")) {
                m_ipinfo.setOrg(jsonObj.value(u"org").toString());
            }
            if (jsonObj.contains(u"postal")) {
                m_ipinfo.setPostal(jsonObj.value(u"postal").toString());
            }
            if (jsonObj.contains(u"timezone")) {
                m_ipinfo.setTimezone(jsonObj.value(u"timezone").toString());
            }
            // Generate the result and emit signal
            emit sigRequestCompleted(m_ipinfo.toHTML());

            // Log updated values
            LogMsg(tr("IPGeoLocation::onNetworkReply() IP %1 City %2 Region %3 Country %4 Hostname %5 Loc %6 Org %7 Postal %8 Timezone %9")
                   .arg(m_ipinfo.ip(), m_ipinfo.city(), m_ipinfo.region(), m_ipinfo.country(),
                        m_ipinfo.hostname(), m_ipinfo.loc(), m_ipinfo.org(), m_ipinfo.postal(), m_ipinfo.timezone()));
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
