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
    m_ui->setupUi(this);

    connect(m_ipgeo, &IPGeoLocation::requestCompleted, 
            this, &NetworkDialog::handleGeoLocationCompleted);
    
    connect(m_ipgeo, &IPGeoLocation::requestFailed, 
            this, &NetworkDialog::handleGeoLocationFailed);


    m_ipgeo->doRequest();

    // Connect signals from NetworkSpeedTest to slots in NetworkDialog
    connect(m_speedTest, &NetworkSpeedTest::speedTestCompleted, 
            this, &NetworkDialog::handleSpeedTestCompleted);
    
    connect(m_speedTest, &NetworkSpeedTest::speedTestFailed, 
            this, &NetworkDialog::handleSpeedTestFailed);

    // Set waiting text and start speed test
    m_ui->ipLocationData->setPlainText(QStringLiteral("WAITING..."));
    LogMsg(tr("NetworkDialog::NetworkDialog() - Starting network speed test."));
    m_speedTest->runSpeedTest();

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
    m_ui->ipLocationData->setPlainText(result);
}

void NetworkDialog::handleSpeedTestFailed(const QString &errorMessage)
{
    LogMsg(tr("NetworkDialog::handleSpeedTestFailed() - Test failed: %1").arg(errorMessage));
    m_ui->ipLocationData->setPlainText(errorMessage);
}

void NetworkDialog::handleGeoLocationCompleted(const QString &result)
{
    LogMsg(tr("NetworkDialog::handleGeoLocationCompleted() - Test successful."));
    m_ui->netspeedData->setPlainText(result);
}

void NetworkDialog::handleGeoLocationFailed(const QString &errorMessage)
{
    LogMsg(tr("NetworkDialog::handleGeoLocationFailed() - Test failed: %1").arg(errorMessage));
    m_ui->netspeedData->setPlainText(errorMessage);
}


QString NetworkDialog::generateIPInfoHTML(const IPInfo &info) const
{
    return u"<html><head/><body><p>"
           u"<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">"
           u"<tr><th>Field</th><th>Value</th></tr>"
           u"<tr><td><b>IP</b></td><td>%1</td></tr>"
           u"<tr><td><b>City</b></td><td>%2</td></tr>"
           u"<tr><td><b>Region</b></td><td>%3</td></tr>"
           u"<tr><td><b>Country</b></td><td>%4</td></tr>"
           u"</table>"
           u"</body></html>"_s
        .arg(info.ip, info.city, info.region, info.country);
}

