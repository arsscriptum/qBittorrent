
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

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void requestCompleted(const QString &result);
    void requestFailed(const QString &errorMessage);


private:
    QProcess *m_process = nullptr;
    QNetworkAccessManager manager;
};

