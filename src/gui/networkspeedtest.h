#pragma once

#include <QObject>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

class NetworkSpeedTest : public QObject
{
    Q_OBJECT

public:
    explicit NetworkSpeedTest(QObject *parent = nullptr);
    void runSpeedTest();
    
    struct SpeedTestResult
    {
        double downloadSpeed;  // in bits per second
        double uploadSpeed;    // in bits per second
        double ping;           // in milliseconds

        // Server Information
        QString serverName;
        QString serverCity;
        QString serverCountry;
        double serverLatency;

        // Client Information
        QString clientIP;
        QString clientISP;
    };

signals:
    void sigSpeedTestCompleted(const QString &result);
    void sigSpeedTestFailed(const QString &errorMessage);

private slots:
    void onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus);
private:
    QString generateSpeedTestHTML(const SpeedTestResult &result) const;
    QProcess *m_process = nullptr;
};
