#include "networkspeedtest.h"
#include <QProcess>
#include <QClipboard>
#include <QApplication>
#include "ipgeolocation.h"
#include <QProcess>
#include "base/global.h"
#include "base/path.h"
#include "base/unicodestrings.h"
#include "base/utils/io.h"
#include "base/utils/misc.h"
#include "base/version.h"
#include "ui_networkdialog.h"
#include "uithememanager.h"
#include "utils.h"
#include "base/global.h"
#include "base/logger.h"


NetworkSpeedTest::NetworkSpeedTest(QObject *parent) 
    : QObject(parent), m_process(new QProcess(this))
{
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &NetworkSpeedTest::onSpeedTestFinished);
}

void NetworkSpeedTest::runSpeedTest()
{
    LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Starting speed test..."));
    
    QString exePath = QStringLiteral("/usr/bin/speedtest-cli");
    QStringList argList;
    argList << QStringLiteral("--json") << QStringLiteral("--no-upload") << QStringLiteral("--bytes");

    m_process->start(exePath, argList);

    if (!m_process->waitForStarted()) {
        LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Error: Could not start speedtest-cli."));
        emit speedTestFailed(QStringLiteral("Error: Could not start speedtest-cli."));
    }
}

void NetworkSpeedTest::onSpeedTestFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    LogMsg(tr("NetworkSpeedTest::onSpeedTestFinished() - Process finished with exit code: %1").arg(exitCode));

    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        LogMsg(tr("NetworkSpeedTest::onSpeedTestFinished() - Process crashed or exited with error."));
        emit speedTestFailed(QStringLiteral("Speed test failed."));
        return;
    }

    // Read the entire output as a QString
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());

    // Log the raw output
    LogMsg(tr("NetworkSpeedTest::onSpeedTestFinished() - Raw output: %1").arg(output));

    QJsonDocument jsonDoc = QJsonDocument::fromJson(output.toUtf8());
    if (!jsonDoc.isObject()) {
        LogMsg(tr("NetworkSpeedTest::onSpeedTestFinished() - Invalid JSON output."));
        emit speedTestFailed(QStringLiteral("Invalid JSON output from speedtest."));
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    double downloadSpeed = jsonObj.value(QStringLiteral("download")).toDouble(-1);
    QJsonObject serverObj = jsonObj.value(QStringLiteral("server")).toObject();
    QString server = serverObj.contains(QStringLiteral("sponsor")) ? 
                     serverObj.value(QStringLiteral("sponsor")).toString() : 
                     QStringLiteral("Unknown Server");
    QString country = serverObj.contains(QStringLiteral("country")) ? 
                      serverObj.value(QStringLiteral("country")).toString() : 
                      QStringLiteral("Unknown Country");

    if (downloadSpeed < 0) {
        LogMsg(tr("NetworkSpeedTest::onSpeedTestFinished() - Download speed missing in JSON."));
        emit speedTestFailed(QStringLiteral("Download speed not available."));
        return;
    }

    QString result = QStringLiteral("Download Speed: %1 Mbps\nServer: %2, %3")
                     .arg(QString::number(downloadSpeed / (1024 * 1024), 'f', 2))
                     .arg(server)
                     .arg(country);

    LogMsg(tr("NetworkSpeedTest::onSpeedTestFinished() - Final result: %1").arg(result));
    emit speedTestCompleted(result);
}
