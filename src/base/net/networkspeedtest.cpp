#include "networkspeedtest.h"
#include "base/global.h"
#include "base/logger.h"
#include "speedtestinfo.h"
NetworkSpeedTest *NetworkSpeedTest::pInstance = nullptr;
// Private Constructor (Singleton)
NetworkSpeedTest::NetworkSpeedTest(QObject *parent) 
    : QObject(parent), m_process(new QProcess(this)), m_state(RequestState::Idle), m_receivedData(false)
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
void NetworkSpeedTest::runSpeedTest(bool force)
{
    static QDateTime lastRequestTime; // Stores the last request timestamp

    // Check if at least 5 minutes (300 seconds) have passed since the last request
    if (!force && lastRequestTime.isValid() && lastRequestTime.secsTo(QDateTime::currentDateTime()) < 300) {
        LogMsg(tr("NetworkSpeedTest::doRequest() Skipping request, last request was too recent."));
        return; // Skip the request if it's too soon
    }

    if (m_state == RequestState::InProgress) {
        LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Speed test already in progress."));
        return;
    }

    LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Starting speed test..."));

    m_state = RequestState::InProgress;  // Update state

    QString exePath = QStringLiteral("/usr/bin/speedtest-cli");
    QStringList argList;
    argList << QStringLiteral("--json") << QStringLiteral("--bytes");

    m_process->start(exePath, argList);

    if (!m_process->waitForStarted()) {
        LogMsg(tr("NetworkSpeedTest::runSpeedTest() - Error: Could not start speedtest-cli."));
        emit sigSpeedTestFailed(QStringLiteral("Error: Could not start speedtest-cli."));
        m_state = RequestState::Idle;  // Reset state on failure
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
        m_state = RequestState::Idle;  // Reset state on failure
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
        m_state = RequestState::Idle;  // Reset state on failure
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // Extract values
    m_speedtestInfo.fromQJsonObject(jsonObj);

    if (m_speedtestInfo.download() < 0) {
        LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Download speed missing in JSON."));
        emit sigSpeedTestFailed(QStringLiteral("Download speed not available."));
        m_state = RequestState::Idle;  // Reset state on failure
        return;
    }

    // Mark that data has been received for the first time
    if (!m_receivedData) {
        m_receivedData = true;
    }

    // Update state
    m_state = RequestState::Completed;

    // Generate HTML result
    QString htmlResult = m_speedtestInfo.toHTML();
    LogMsg(tr("NetworkSpeedTest::onProcessTerminated() - Final result generated."));

    // Emit signal with HTML result
    emit sigSpeedTestCompleted(htmlResult);
}
