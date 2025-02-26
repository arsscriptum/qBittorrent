
#include "ipinfo.h"

void IPInfo::reset()
{
    m_ip.clear();
    m_city.clear();
    m_region.clear();
    m_country.clear();
    m_loc.clear();
    m_org.clear();
    m_timezone.clear();
    m_hostname.clear();
    m_postal.clear();
}

QJsonObject IPInfo::toQJsonObject() const
{
    QJsonObject jsonObj;
    jsonObj.insert(u"ip", m_ip);
    jsonObj.insert(u"city", m_city);
    jsonObj.insert(u"region", m_region);
    jsonObj.insert(u"country", m_country);
    jsonObj.insert(u"loc", m_loc);
    jsonObj.insert(u"org", m_org);
    jsonObj.insert(u"timezone", m_timezone);
    jsonObj.insert(u"hostname", m_hostname);
    jsonObj.insert(u"postal", m_postal);
    return jsonObj;
}

// Populate IPInfo from QJsonObject
void IPInfo::fromQJsonObject(const QJsonObject &jsonObj)
{
    if (jsonObj.contains(u"ip"))
        m_ip = jsonObj.value(u"ip").toString();
    if (jsonObj.contains(u"city"))
        m_city = jsonObj.value(u"city").toString();
    if (jsonObj.contains(u"region"))
        m_region = jsonObj.value(u"region").toString();
    if (jsonObj.contains(u"country"))
        m_country = jsonObj.value(u"country").toString();
    if (jsonObj.contains(u"loc"))
        m_loc = jsonObj.value(u"loc").toString();
    if (jsonObj.contains(u"org"))
        m_org = jsonObj.value(u"org").toString();
    if (jsonObj.contains(u"timezone"))
        m_timezone = jsonObj.value(u"timezone").toString();
    if (jsonObj.contains(u"hostname"))
        m_hostname = jsonObj.value(u"hostname").toString();
    if (jsonObj.contains(u"postal"))
        m_postal = jsonObj.value(u"postal").toString();
}
QString IPInfo::toHTML() const
{
    return QStringLiteral(
        "<html><head/><body><p>"
        "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">"
        "<tr><th>Field</th><th>Value</th></tr>"
        "<tr><td><b>IP</b></td><td>%1</td></tr>"
        "<tr><td><b>City</b></td><td>%2</td></tr>"
        "<tr><td><b>Region</b></td><td>%3</td></tr>"
        "<tr><td><b>Country</b></td><td>%4</td></tr>"
        "<tr><td><b>Hostname</b></td><td>%5</td></tr>"
        "<tr><td><b>Location</b></td><td>%6</td></tr>"
        "<tr><td><b>Organization</b></td><td>%7</td></tr>"
        "<tr><td><b>Postal</b></td><td>%8</td></tr>"
        "<tr><td><b>Timezone</b></td><td>%9</td></tr>"
        "</table>"
        "</body></html>")
        .arg(m_ip, m_city, m_region, m_country, m_hostname, m_loc, m_org, m_postal, m_timezone);
}
