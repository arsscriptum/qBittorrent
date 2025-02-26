#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
class SpeedTestInfo
{
public:
    explicit SpeedTestInfo();
    void reset();

    QString toQString() const;
    QJsonObject toQJsonObject() const;
    QString toHTML() const;

    void fromQJsonObject(const QJsonObject &jsonObj);

    // Getters
    double download() const { return m_download; }
    double upload() const { return m_upload; }
    double ping() const { return m_ping; }
    QString timestamp() const { return m_timestamp; }

    QString serverUrl() const { return m_serverUrl; }
    QString serverName() const { return m_serverName; }
    QString serverCountry() const { return m_serverCountry; }
    QString serverSponsor() const { return m_serverSponsor; }
    double serverLatency() const { return m_serverLatency; }

    QString clientIp() const { return m_clientIp; }
    QString clientIsp() const { return m_clientIsp; }
    QString clientCountry() const { return m_clientCountry; }

    // Setters
    void setDownload(double download) { m_download = download; }
    void setUpload(double upload) { m_upload = upload; }
    void setPing(double ping) { m_ping = ping; }
    void setTimestamp(const QString &timestamp) { m_timestamp = timestamp; }

    void setServerUrl(const QString &url) { m_serverUrl = url; }
    void setServerName(const QString &name) { m_serverName = name; }
    void setServerCountry(const QString &country) { m_serverCountry = country; }
    void setServerSponsor(const QString &sponsor) { m_serverSponsor = sponsor; }
    void setServerLatency(double latency) { m_serverLatency = latency; }

    void setClientIp(const QString &ip) { m_clientIp = ip; }
    void setClientIsp(const QString &isp) { m_clientIsp = isp; }
    void setClientCountry(const QString &country) { m_clientCountry = country; }

private:
    double m_download;
    double m_upload;
    double m_ping;
    QString m_timestamp;

    QString m_serverUrl;
    QString m_serverName;
    QString m_serverCountry;
    QString m_serverSponsor;
    double m_serverLatency;

    QString m_clientIp;
    QString m_clientIsp;
    QString m_clientCountry;
};
