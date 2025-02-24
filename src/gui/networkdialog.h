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

#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QProcess>
#include "base/settingvalue.h"


namespace Ui
{
    class NetworkDialog;
}




class NetworkDialog final : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NetworkDialog)

public:
    explicit NetworkDialog(QWidget *parent);
    ~NetworkDialog() override;


    struct IPInfo
    {
        QString ip;
        QString city;
        QString region;
        QString country;

        // Convert IPInfo to a const QString
        QString toQString() const
        {
            return QStringLiteral("IP: %1\nCity: %2\nRegion: %3\nCountry: %4")
                .arg(ip, city, region, country);
        }
    };

private slots:
    void handleSpeedTestCompleted(const QString &result);
    void handleSpeedTestFailed(const QString &errorMessage);
    void handleGeoLocationCompleted(const QString &result);
    void handleGeoLocationFailed(const QString &errorMessage);
    
private:

    QString generateIPInfoHTML(const IPInfo &info) const;
    Ui::NetworkDialog *m_ui;
    SettingValue<QSize> m_storeDialogSize;
    class NetworkSpeedTest *m_speedTest;
    class IPGeoLocation *m_ipgeo;
};
