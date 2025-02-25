
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



class IPGeoLocation : public QObject
{
    Q_OBJECT

public:
    explicit IPGeoLocation(QObject *parent = nullptr);
    void doRequest();

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
    void onNetworkReply(QNetworkReply *reply);
    void onProcessTerminated(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void sigRequestCompleted(const QString &result);
    void sigRequestFailed(const QString &errorMessage);


private:
    QString generateIPInfoHTML(const IPInfo &info) const;

    QProcess *m_process = nullptr;
    QNetworkAccessManager manager;
};

