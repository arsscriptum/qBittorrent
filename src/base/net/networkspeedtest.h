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
    // Enum for request state
    enum class RequestState {
        Idle,
        InProgress,
        Completed
    };

    // Singleton instance
    static NetworkSpeedTest &instance()
    {
        if(pInstance == nullptr)
        {
            pInstance = new NetworkSpeedTest();
        } 
        return *pInstance;
    }

    // Delete copy constructor & assignment to enforce singleton
    NetworkSpeedTest(const NetworkSpeedTest &) = delete;
    NetworkSpeedTest &operator=(const NetworkSpeedTest &) = delete;

    void runSpeedTest();

    // Getters for speed test info and new state variables
    SpeedTestInfo getSpeedTestResults() const { return m_speedtestInfo; }
    RequestState currentState() const { return m_state; }
    bool hasReceivedData() const { return m_receivedData; }

signals:
    void sigSpeedTestCompleted(const QString &result);
    void sigSpeedTestFailed(const QString &errorMessage);

private slots:
    void onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus);

private:
    static NetworkSpeedTest *pInstance;
    explicit NetworkSpeedTest(QObject *parent = nullptr); // Private constructor
    ~NetworkSpeedTest() override; // Destructor

    SpeedTestInfo m_speedtestInfo;
    QProcess *m_process = nullptr;
    RequestState m_state = RequestState::Idle;  // Default state
    bool m_receivedData = false;  // Tracks if data has been received at least once
};
