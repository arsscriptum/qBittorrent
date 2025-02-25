/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2006  Christophe Dumez <chris@qbittorrent.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "networkdialog.h"

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
#include "networkspeedtest.h"
#include "base/global.h"
#include "base/logger.h"

#define SETTINGS_KEY(name) u"NetworkDialog/" name

NetworkDialog::NetworkDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::NetworkDialog)
    , m_storeDialogSize(SETTINGS_KEY(u"Size"_s))
    , m_speedTest(new NetworkSpeedTest(this))
    , m_ipgeo(new IPGeoLocation(this))
{
    _enableProgress=true;
    m_ui->setupUi(this);
    QString progressHtml1 = QStringLiteral(
        "<html><head><style>"
        "body { font-family: Arial, sans-serif; text-align: center; }"
        "progress { width: 80%%; height: 30px; }"
        "</style></head><body>"
        "<h2>Running IP Geo Location...</h2>"
        "<progress value='0' max='100'></progress>"
        "<p>Waiting...</p>"
        "</body></html>"
    );
    QString progressHtml2 = QStringLiteral(
        "<html><head><style>"
        "body { font-family: Arial, sans-serif; text-align: center; }"
        "progress { width: 80%%; height: 30px; }"
        "</style></head><body>"
        "<h2>Running Speed Test...</h2>"
        "<progress value='0' max='100'></progress>"
        "<p>Waiting...</p>"
        "</body></html>"
    );
    startTextProgress();
    m_ui->ipLocationData->setHtml(progressHtml1);
    m_ui->netspeedData->setHtml(progressHtml2);


    connect(m_ipgeo, &IPGeoLocation::sigRequestCompleted, 
            this, &NetworkDialog::handleGeoLocationCompleted);
    
    connect(m_ipgeo, &IPGeoLocation::sigRequestFailed, 
            this, &NetworkDialog::handleGeoLocationFailed);
    
    // Connect signals from NetworkSpeedTest to slots in NetworkDialog
    connect(m_speedTest, &NetworkSpeedTest::sigSpeedTestCompleted, 
            this, &NetworkDialog::handleSpeedTestCompleted);
    
    connect(m_speedTest, &NetworkSpeedTest::sigSpeedTestFailed, 
            this, &NetworkDialog::handleSpeedTestFailed);

    
    
    LogMsg(tr("NetworkDialog::NetworkDialog() - Starting network speed test."));
    m_speedTest->runSpeedTest();

    LogMsg(tr("NetworkDialog::NetworkDialog() - Starting ipGeolocalisation::doRequest"));
    m_ipgeo->doRequest();


    if (const auto readResult = Utils::IO::readFile(Path(u":/ipinfo.html"_s), -1, QIODevice::Text)){
            LogMsg(tr("NetworkDialog::NetworkDialog() readFile %1").arg(QString::fromUtf8(readResult.value())));
    }else{
        LogMsg(tr("NetworkDialog::NetworkDialog() readFile failed"));
    }

    if (const QSize dialogSize = m_storeDialogSize; dialogSize.isValid())
        resize(dialogSize);
}

NetworkDialog::~NetworkDialog()
{
    m_storeDialogSize = size();
    delete m_ui;
}


void NetworkDialog::handleSpeedTestCompleted(const QString &result)
{
    LogMsg(tr("NetworkDialog::handleSpeedTestCompleted() - Test successful."));
    stopTextProgress();
    m_ui->netspeedData->setHtml(result);
}

void NetworkDialog::handleSpeedTestFailed(const QString &errorMessage)
{
    LogMsg(tr("NetworkDialog::handleSpeedTestFailed() - Test failed: %1").arg(errorMessage));
    stopTextProgress();
    m_ui->netspeedData->setHtml(errorMessage);
}

void NetworkDialog::handleGeoLocationCompleted(const QString &result)
{
    LogMsg(tr("NetworkDialog::handleGeoLocationCompleted() - Test successful."));
    m_ui->ipLocationData->setHtml(result);
}

void NetworkDialog::handleGeoLocationFailed(const QString &errorMessage)
{
    LogMsg(tr("NetworkDialog::handleGeoLocationFailed() - Test failed: %1").arg(errorMessage));
    m_ui->ipLocationData->setHtml(errorMessage);
}

void NetworkDialog::startTextProgress()
{
    if(!_enableProgress){
        return;
    }
    // Initial progress
    int *progress = new int(0);

    // Initial progress bar with all squares as
    QString progressBar = QStringLiteral("⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜");

    // Timer to update progress every 150ms (15 sec total)
    m_progressTimer = new QTimer(this);
    connect(m_progressTimer, &QTimer::timeout, this, [this, progress, progressBar]() mutable {
        if (*progress >= 100) {
            m_progressTimer->stop();
            m_ui->netspeedData->setPlainText(QStringLiteral("\n\t     Progress: 100%\n\n\t  ⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛\n\n  Speed Test: calculating results... Please wait..."));
            delete progress;  // Clean up dynamically allocated int
        } else {
            int blocksFilled = *progress / 10; // Determine how many blocks to replace

            // Update progress bar
            QString updatedBar;
            for (int i = 0; i < 10; ++i) {
                updatedBar += (i < blocksFilled) ? QStringLiteral("⬛") : QStringLiteral("⬜");
            }
            int tmpProgress = (*progress);
            QString sCurrentState;
            if(tmpProgress < 30){
                sCurrentState = QStringLiteral("Getting the best server for testing...");
            }else if(tmpProgress < 60){
                sCurrentState = QStringLiteral("Testing Download Speed...");
            }else if(tmpProgress < 90){
                sCurrentState = QStringLiteral("Testing Upload Speed...");
            }else{
                sCurrentState = QStringLiteral("Gathering All Results...");
            }

            // Update display
            m_ui->netspeedData->setPlainText(QStringLiteral("\n\t       Progress: %1%\n\n\t  %2\n\n\t    %3").arg(*progress).arg(updatedBar).arg(sCurrentState));

            // Increment progress
            (*progress)++;
        }
    });

    m_progressTimer->start(700); // Runs every 150ms
}

void NetworkDialog::stopTextProgress()
{
    _enableProgress = false;
    if (m_progressTimer) {
        disconnect(m_progressTimer, &QTimer::timeout, nullptr, nullptr);
        m_progressTimer->stop();
        m_progressTimer->deleteLater();
        m_progressTimer = nullptr;      
        m_ui->ipLocationData->setPlainText(QStringLiteral(""));
    }
}
