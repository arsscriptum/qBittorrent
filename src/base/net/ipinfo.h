#ifndef IPINFO_H
#define IPINFO_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

class IPInfo
{
public:
    // Constructor
    IPInfo() { reset(); }

    // Reset all values to empty
    void reset();

    // Convert IPInfo to a human-readable QString
    QString toQString() const
    {
        return QStringLiteral("IP: %1\nCity: %2\nRegion: %3\nCountry: %4\nLoc: %5\nOrg: %6\nTimezone: %7\nHostname: %8\nPostal: %9")
            .arg(m_ip, m_city, m_region, m_country, m_loc, m_org, m_timezone, m_hostname, m_postal);
    }

    // Convert IPInfo to QJsonObject
    QJsonObject toQJsonObject() const;

    QString toHTML() const;
    // Populate IPInfo from QJsonObject
    void fromQJsonObject(const QJsonObject &jsonObj);

    // Getters
    QString ip() const { return m_ip; }
    QString city() const { return m_city; }
    QString region() const { return m_region; }
    QString country() const { return m_country; }
    QString loc() const { return m_loc; }
    QString org() const { return m_org; }
    QString timezone() const { return m_timezone; }
    QString hostname() const { return m_hostname; }
    QString postal() const { return m_postal; }

    // Setters
    void setIp(const QString &ip) { m_ip = ip; }
    void setCity(const QString &city) { m_city = city; }
    void setRegion(const QString &region) { m_region = region; }
    void setCountry(const QString &country) { m_country = country; }
    void setLoc(const QString &loc) { m_loc = loc; }
    void setOrg(const QString &org) { m_org = org; }
    void setTimezone(const QString &timezone) { m_timezone = timezone; }
    void setHostname(const QString &hostname) { m_hostname = hostname; }
    void setPostal(const QString &postal) { m_postal = postal; }

private:
    QString m_ip;
    QString m_city;
    QString m_region;
    QString m_country;
    QString m_loc;
    QString m_org;
    QString m_timezone;
    QString m_hostname;
    QString m_postal;
};

#endif // IPINFO_H
