#include "speedtestinfo.h"

SpeedTestInfo::SpeedTestInfo()
{
    reset();
}

// Reset all values
void SpeedTestInfo::reset()
{
    m_download = 0.0;
    m_upload = 0.0;
    m_ping = 0.0;
    m_timestamp.clear();

    m_serverUrl.clear();
    m_serverName.clear();
    m_serverCountry.clear();
    m_serverSponsor.clear();
    m_serverLatency = 0.0;

    m_clientIp.clear();
    m_clientIsp.clear();
    m_clientCountry.clear();
}

// Convert to a human-readable string
QString SpeedTestInfo::toQString() const
{
    return QStringLiteral(
        "Download: %1 Mbps\nUpload: %2 Mbps\nPing: %3 ms\nTimestamp: %4\n"
        "Server: %5 (%6, %7) - Sponsored by %8, Latency: %9 ms\n"
        "Client: %10 (%11, %12)"
    ).arg(m_download).arg(m_upload).arg(m_ping).arg(m_timestamp)
     .arg(m_serverName, m_serverCountry, m_serverUrl, m_serverSponsor)
     .arg(m_serverLatency)
     .arg(m_clientIp, m_clientIsp, m_clientCountry);
}

// Convert to QJsonObject
QJsonObject SpeedTestInfo::toQJsonObject() const
{
    QJsonObject jsonObj;
    jsonObj.insert(u"download", m_download);
    jsonObj.insert(u"upload", m_upload);
    jsonObj.insert(u"ping", m_ping);
    jsonObj.insert(u"timestamp", m_timestamp);

    QJsonObject serverObj;
    serverObj.insert(u"url", m_serverUrl);
    serverObj.insert(u"name", m_serverName);
    serverObj.insert(u"country", m_serverCountry);
    serverObj.insert(u"sponsor", m_serverSponsor);
    serverObj.insert(u"latency", m_serverLatency);

    QJsonObject clientObj;
    clientObj.insert(u"ip", m_clientIp);
    clientObj.insert(u"isp", m_clientIsp);
    clientObj.insert(u"country", m_clientCountry);

    jsonObj.insert(u"server", serverObj);
    jsonObj.insert(u"client", clientObj);
    return jsonObj;
}

// Convert to HTML table format
QString SpeedTestInfo::toHTML() const
{
    return QStringLiteral(
        "<html><head/><body><p>"
        "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">"
        "<tr><th>Field</th><th>Value</th></tr>"
        "<tr><td><b>Download</b></td><td>%1 Mbps</td></tr>"
        "<tr><td><b>Upload</b></td><td>%2 Mbps</td></tr>"
        "<tr><td><b>Ping</b></td><td>%3 ms</td></tr>"
        "<tr><td><b>Timestamp</b></td><td>%4</td></tr>"
        "<tr><td colspan=\"2\"><b>Server Information</b></td></tr>"
        "<tr><td><b>Name</b></td><td>%5</td></tr>"
        "<tr><td><b>Country</b></td><td>%6</td></tr>"
        "<tr><td><b>URL</b></td><td>%7</td></tr>"
        "<tr><td><b>Sponsor</b></td><td>%8</td></tr>"
        "<tr><td><b>Latency</b></td><td>%9 ms</td></tr>"
        "<tr><td colspan=\"2\"><b>Client Information</b></td></tr>"
        "<tr><td><b>IP</b></td><td>%10</td></tr>"
        "<tr><td><b>ISP</b></td><td>%11</td></tr>"
        "<tr><td><b>Country</b></td><td>%12</td></tr>"
        "</table>"
        "</body></html>"
    ).arg(m_download).arg(m_upload).arg(m_ping).arg(m_timestamp)
     .arg(m_serverName, m_serverCountry, m_serverUrl, m_serverSponsor)
     .arg(m_serverLatency)
     .arg(m_clientIp, m_clientIsp, m_clientCountry);
}

// Populate from QJsonObject
void SpeedTestInfo::fromQJsonObject(const QJsonObject &jsonObj)
{
    if (jsonObj.contains(u"download")) m_download = jsonObj[u"download"].toDouble();
    if (jsonObj.contains(u"upload")) m_upload = jsonObj[u"upload"].toDouble();
    if (jsonObj.contains(u"ping")) m_ping = jsonObj[u"ping"].toDouble();
    if (jsonObj.contains(u"timestamp")) m_timestamp = jsonObj[u"timestamp"].toString();

    if (jsonObj.contains(u"server"))
    {
        QJsonObject server = jsonObj[u"server"].toObject();
        if (server.contains(u"url")) m_serverUrl = server[u"url"].toString();
        if (server.contains(u"name")) m_serverName = server[u"name"].toString();
        if (server.contains(u"country")) m_serverCountry = server[u"country"].toString();
        if (server.contains(u"sponsor")) m_serverSponsor = server[u"sponsor"].toString();
        if (server.contains(u"latency")) m_serverLatency = server[u"latency"].toDouble();
    }

    if (jsonObj.contains(u"client"))
    {
        QJsonObject client = jsonObj[u"client"].toObject();
        if (client.contains(u"ip")) m_clientIp = client[u"ip"].toString();
        if (client.contains(u"isp")) m_clientIsp = client[u"isp"].toString();
        if (client.contains(u"country")) m_clientCountry = client[u"country"].toString();
    }
}
