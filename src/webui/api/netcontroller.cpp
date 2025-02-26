/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2018  Vladimir Golovnev <glassez@yandex.ru>
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

#include "netcontroller.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QThread>
#include <QList>
#include "base/logger.h"
#include "base/utils/string.h"
#include "apierror.h"
#include "base/net/ipinfo.h"
#include "base/net/networkspeedtest.h"

using Utils::String::parseBool;
#include "netcontroller.h"
#include "base/net/ipgeolocation.h"
#include <QDebug>

// Constructor - Fix by matching APIController's constructor
NETController::NETController(IApplication *app, QObject *parent)
    : APIController(app, parent), m_updated(false)  // Call base class constructor correctly
{
    qDebug() << "NETController initialized.";
    refreshNetSpeedAction();
    refreshIpInfoAction();
}

// Destructor - Fix by overriding virtual destructor
NETController::~NETController()
{
    qDebug() << "NETController destroyed.";

    // Get the singleton instance of IPGeoLocation

}


void NETController::getIpInfoAction()
{
    const IPInfo ipinfo = IPGeoLocation::instance().getIpInfo();
    LogMsg(tr("NETController::getIpInfoAction %1").arg(ipinfo.toQString()));
    setResult(ipinfo.toQJsonObject());
}


void NETController::refreshIpInfoAction()
{
    IPGeoLocation &geo = IPGeoLocation::instance();
        // Connect signals for success and failure handling
    connect(&geo, &IPGeoLocation::sigRequestCompleted, 
            this, &NETController::handleGeoLocationCompleted);

    connect(&geo, &IPGeoLocation::sigRequestFailed, 
            this, &NETController::handleGeoLocationFailed);

    LogMsg(tr("NETController::refreshIpInfoAction"));
    IPGeoLocation::instance().doRequest(true);
    m_updated = false;
    static QDateTime sentTime; // Stores the last request timestamp

    // Update the last request timestamp
    sentTime = QDateTime::currentDateTime();

    while(!m_updated){
        if(sentTime.secsTo(QDateTime::currentDateTime()) < 10){
            break;
        }
        QThread::usleep(150000);  // Sleep for 150 millisecond
    }
    const IPInfo ipinfo = IPGeoLocation::instance().getIpInfo();
    setResult(ipinfo.toQJsonObject());
}   





void NETController::getNetSpeedAction()
{
    const SpeedTestInfo results = NetworkSpeedTest::instance().getSpeedTestResults();
    LogMsg(tr("NETController::getNetSpeedAction %1").arg(results.toQString()));
    setResult(results.toQJsonObject());
}


void NETController::refreshNetSpeedAction()
{
    NetworkSpeedTest &netTester = NetworkSpeedTest::instance();
        // Connect signals for success and failure handling
    // Connect signals from NetworkSpeedTest to slots in NetworkDialog
    connect(&netTester, &NetworkSpeedTest::sigSpeedTestCompleted, 
            this, &NETController::handleSpeedTestCompleted);
    
    connect(&netTester, &NetworkSpeedTest::sigSpeedTestFailed, 
            this, &NETController::handleSpeedTestFailed);

    LogMsg(tr("NETController::refreshNetSpeedAction"));
    NetworkSpeedTest::instance().runSpeedTest();
    m_updated = false;
    static QDateTime sentTime; // Stores the last request timestamp

    // Update the last request timestamp
    sentTime = QDateTime::currentDateTime();

    while(!m_updated){
        if(sentTime.secsTo(QDateTime::currentDateTime()) < 10){
            break;
        }
        QThread::usleep(150000);  // Sleep for 150 millisecond
    }
    const IPInfo ipinfo = IPGeoLocation::instance().getIpInfo();
    setResult(ipinfo.toQJsonObject());
}   


void NETController::handleGeoLocationCompleted(const QString &result)
{
    
    IPGeoLocation &geo = IPGeoLocation::instance();
    // Disconnect signals to prevent dangling connections
    disconnect(&geo, &IPGeoLocation::sigRequestCompleted, 
               this, &NETController::handleGeoLocationCompleted);

    disconnect(&geo, &IPGeoLocation::sigRequestFailed, 
               this, &NETController::handleGeoLocationFailed);
    LogMsg(tr("NETController::handleGeoLocationCompleted() - %1.").arg(result));
    m_updated = true;
}

void NETController::handleGeoLocationFailed(const QString &errorMessage)
{
    IPGeoLocation &geo = IPGeoLocation::instance();

    // Disconnect signals to prevent dangling connections
    disconnect(&geo, &IPGeoLocation::sigRequestCompleted, 
               this, &NETController::handleGeoLocationCompleted);

    disconnect(&geo, &IPGeoLocation::sigRequestFailed, 
               this, &NETController::handleGeoLocationFailed);
    LogMsg(tr("NETController::handleGeoLocationFailed() - Test failed: %1").arg(errorMessage));
    m_updated = true;
}

void NETController::handleSpeedTestCompleted(const QString &result)
{
    LogMsg(tr("NETController::handleSpeedTestCompleted() - Test successful %1").arg(result));
    NetworkSpeedTest &netTester = NetworkSpeedTest::instance();

    // Disconnect signals to prevent dangling connections
    disconnect(&netTester, &NetworkSpeedTest::sigSpeedTestCompleted, 
               this, &NETController::handleSpeedTestCompleted);

    disconnect(&netTester, &NetworkSpeedTest::sigSpeedTestFailed, 
               this, &NETController::handleSpeedTestFailed);

    m_updated = true;          
}

void NETController::handleSpeedTestFailed(const QString &errorMessage)
{
    LogMsg(tr("NETController::handleSpeedTestFailed() - Test failed: %1").arg(errorMessage));
    NetworkSpeedTest &netTester = NetworkSpeedTest::instance();

    // Disconnect signals to prevent dangling connections
    disconnect(&netTester, &NetworkSpeedTest::sigSpeedTestCompleted, 
               this, &NETController::handleSpeedTestCompleted);

    disconnect(&netTester, &NetworkSpeedTest::sigSpeedTestFailed, 
               this, &NETController::handleSpeedTestFailed);

    m_updated = true;
}
