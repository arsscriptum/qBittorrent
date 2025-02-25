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
            this, &NetworkSpeedTest::onProcessTerminated);
}

void NetworkSpeedTest::runSpeedTest()
{
    LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Starting speed test..."));
    
    QString exePath = QStringLiteral("/usr/bin/speedtest-cli");
    //QString exePath = QStringLiteral("/home/gp/dev/qBittorrent/scripts/speedtest.sh");
    QStringList argList;
    argList << QStringLiteral("--json") << QStringLiteral("--bytes");

    m_process->start(exePath, argList);

    if (!m_process->waitForStarted()) {
        LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Error: Could not start speedtest-cli."));
        emit sigSpeedTestFailed(QStringLiteral("Error: Could not start speedtest-cli."));
    }else{
         LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Process Started"));
    }
}

void NetworkSpeedTest::onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus)
{
    LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Process finished with exit code: %1").arg(exitCode));

    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Process crashed or exited with error."));
        emit sigSpeedTestFailed(QStringLiteral("Speed test failed."));
        return;
    }

    // Read the entire output as a QString
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());

    // Log the raw output
    LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Raw output: %1").arg(output));

    // Parse JSON output
    QJsonDocument jsonDoc = QJsonDocument::fromJson(output.toUtf8());
    if (!jsonDoc.isObject()) {
        LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Invalid JSON output."));
        emit sigSpeedTestFailed(QStringLiteral("Invalid JSON output from speedtest."));
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // Extract values
    SpeedTestResult result;
    result.downloadSpeed = jsonObj.value(QStringLiteral("download")).toDouble(-1);
    result.uploadSpeed = jsonObj.value(QStringLiteral("upload")).toDouble(-1);
    result.ping = jsonObj.value(QStringLiteral("ping")).toDouble(-1);

    QJsonObject serverObj = jsonObj.value(QStringLiteral("server")).toObject();
    result.serverName = serverObj.value(QStringLiteral("sponsor")).toString(QStringLiteral("Unknown Server"));
    result.serverCity = serverObj.value(QStringLiteral("name")).toString(QStringLiteral("Unknown City"));
    result.serverCountry = serverObj.value(QStringLiteral("country")).toString(QStringLiteral("Unknown Country"));
    result.serverLatency = serverObj.value(QStringLiteral("latency")).toDouble(-1);

    QJsonObject clientObj = jsonObj.value(QStringLiteral("client")).toObject();
    result.clientIP = clientObj.value(QStringLiteral("ip")).toString(QStringLiteral("Unknown IP"));
    result.clientISP = clientObj.value(QStringLiteral("isp")).toString(QStringLiteral("Unknown ISP"));

    // Validate download speed
    if (result.downloadSpeed < 0) {
        LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Download speed missing in JSON."));
        emit sigSpeedTestFailed(QStringLiteral("Download speed not available."));
        return;
    }

    // Generate HTML result using the new function
    QString htmlResult = generateSpeedTestHTML(result);

    LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Final result generated."));
    
    // Emit the signal with the formatted HTML
    emit sigSpeedTestCompleted(htmlResult);
}



QString NetworkSpeedTest::generateSpeedTestHTML(const NetworkSpeedTest::SpeedTestResult &result) const
{
    return u"<html><head/><body><p>"
           u"<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">"
           u"<tr><th colspan=\"2\"><b>Speed Test Results</b></th></tr>"
           u"<tr><td><b>Download Speed</b></td><td>%1 Mbps</td></tr>"
           u"<tr><td><b>Upload Speed</b></td><td>%2 Mbps</td></tr>"
           u"<tr><td><b>Ping</b></td><td>%3 ms</td></tr>"
           u"<tr><th colspan=\"2\"><b>Server Information</b></th></tr>"
           u"<tr><td><b>Server Name</b></td><td>%4</td></tr>"
           u"<tr><td><b>Location</b></td><td>%5, %6</td></tr>"
           u"<tr><td><b>Latency</b></td><td>%7 ms</td></tr>"
           u"<tr><th colspan=\"2\"><b>Client Information</b></th></tr>"
           u"<tr><td><b>IP Address</b></td><td>%8</td></tr>"
           u"<tr><td><b>ISP</b></td><td>%9</td></tr>"
           u"</table>"
           u"</body></html>"_s
        .arg(QString::number(result.downloadSpeed / 1'000'000, 'f', 2), // Convert bps to Mbps
             QString::number(result.uploadSpeed / 1'000'000, 'f', 2),   // Convert bps to Mbps
             QString::number(result.ping, 'f', 2),
             result.serverName,
             result.serverCity,
             result.serverCountry,
             QString::number(result.serverLatency, 'f', 2),
             result.clientIP,
             result.clientISP);
}
