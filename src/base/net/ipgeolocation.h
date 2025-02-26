#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QProcess>
#include "base/settingvalue.h"
#include "base/net/ipinfo.h"

class IPGeoLocation : public QObject
{
    Q_OBJECT

public:
    // Get the singleton instance
    static IPGeoLocation &instance()
    {
        if(pInstance == nullptr)
        {
            pInstance = new IPGeoLocation();
        } 
        return *pInstance;
    }

    // Delete copy constructor & assignment operator to enforce singleton pattern
    IPGeoLocation(const IPGeoLocation &) = delete;
    IPGeoLocation &operator=(const IPGeoLocation &) = delete;

    void doRequest(bool force = false);
    IPInfo getIpInfo() const { return m_ipinfo; }
    void SetRequestDelay(int d) { m_delay = d; }

signals:
    void sigRequestCompleted(const QString &result);
    void sigRequestFailed(const QString &errorMessage);

private slots:
    void onNetworkReply(QNetworkReply *reply);
    void onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus);

private:
    static IPGeoLocation *pInstance;
    // Private constructor to prevent external instantiation
    explicit IPGeoLocation(QObject *parent = nullptr);

    QString generateIPInfoHTML(const IPInfo &info) const;
    IPInfo m_ipinfo;
    int m_delay = 0;
    QProcess *m_process = nullptr;
    QNetworkAccessManager manager;
};
