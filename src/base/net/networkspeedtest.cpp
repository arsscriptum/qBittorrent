#include "networkspeedtest.h"
#include "base/global.h"
#include "base/logger.h"
#include "speedtestinfo.h"

// Private Constructor (Singleton)
NetworkSpeedTest::NetworkSpeedTest(QObject *parent) 
    : QObject(parent), m_process(new QProcess(this))
{
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &NetworkSpeedTest::onProcessTerminated);
}

// Destructor
NetworkSpeedTest::~NetworkSpeedTest()
{
    if (m_process)
    {
        m_process->kill();  // Ensure process stops
        m_process->deleteLater();
    }
}

// Run Speed Test
void NetworkSpeedTest::runSpeedTest()
{
    LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Starting speed test..."));

    QString exePath = QStringLiteral("/usr/bin/speedtest-cli");
    QStringList argList;
    argList << QStringLiteral("--json") << QStringLiteral("--bytes");

    m_process->start(exePath, argList);

    if (!m_process->waitForStarted()) {
        LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Error: Could not start speedtest-cli."));
        emit sigSpeedTestFailed(QStringLiteral("Error: Could not start speedtest-cli."));
    } else {
        LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Process Started"));
    }
}

// Handle Process Completion
void NetworkSpeedTest::onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus)
{
    LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Process finished with exit code: %1").arg(exitCode));

    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Process crashed or exited with error."));
        emit sigSpeedTestFailed(QStringLiteral("Speed test failed."));
        return;
    }

    // Read output
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());

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
    SpeedTestInfo result;
    result.fromQJsonObject(jsonObj);

    if (result.download() < 0) {
        LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Download speed missing in JSON."));
        emit sigSpeedTestFailed(QStringLiteral("Download speed not available."));
        return;
    }

    // Generate HTML result
    QString htmlResult = result.toHTML();
    LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Final result generated."));

    // Emit signal with HTML result
    emit sigSpeedTestCompleted(htmlResult);
}
