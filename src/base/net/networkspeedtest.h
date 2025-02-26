#pragma once

#include <QObject>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "speedtestinfo.h"

class NetworkSpeedTest : public QObject
{
    Q_OBJECT

public:
    // Singleton instance
    static NetworkSpeedTest &instance()
    {
        static NetworkSpeedTest instance;
        return instance;
    }

    // Delete copy constructor & assignment to enforce singleton
    NetworkSpeedTest(const NetworkSpeedTest &) = delete;
    NetworkSpeedTest &operator=(const NetworkSpeedTest &) = delete;

    void runSpeedTest();

    SpeedTestInfo getSpeedTestResults() const { return m_speedtestInfo; }

signals:
    void sigSpeedTestCompleted(const QString &result);
    void sigSpeedTestFailed(const QString &errorMessage);

private slots:
    void onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus);

private:
    explicit NetworkSpeedTest(QObject *parent = nullptr); // Private constructor
    ~NetworkSpeedTest() override; // Destructor
    SpeedTestInfo m_speedtestInfo;
    QProcess *m_process = nullptr;
};
