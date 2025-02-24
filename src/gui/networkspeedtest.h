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

signals:
    void speedTestCompleted(const QString &result);
    void speedTestFailed(const QString &errorMessage);

private slots:
    void onSpeedTestFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *m_process = nullptr;
};
