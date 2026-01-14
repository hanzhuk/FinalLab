#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

    void syncMedicineDatabase();
    void syncDiagnosisReference();
    void backupDatabase();
    void downloadUpdates();

    void setServerUrl(const QString &url) { m_serverUrl = url; }
    void setAuthToken(const QString &token) { m_authToken = token; }

signals:
    void syncStarted(QString operation);
    void syncProgress(int percentage);
    void syncCompleted(QString message);
    void syncFailed(QString error);
    void dataReceived(QJsonObject data);
    void backupProgress(int bytesSent, int totalBytes);

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onUploadProgress(qint64 bytesSent, qint64 totalBytes);
    void onDownloadProgress(qint64 bytesReceived, qint64 totalBytes);

private:
    void handleMedicineSyncResponse(const QJsonObject &response);
    void handleDiagnosisResponse(const QJsonObject &response);
    void handleBackupResponse(const QJsonObject &response);
    void handleUpdateResponse(const QJsonObject &response);

    QNetworkAccessManager *m_networkManager;
    QString m_serverUrl;
    QString m_authToken;
    QString m_currentOperation;
    QNetworkReply *m_currentReply;
};

#endif // NETWORKMANAGER_H
