#include "networkmanager.h"
#include "idatabase.h"
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
{
    m_serverUrl = "http://127.0.0.1:8000";

    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &NetworkManager::onReplyFinished);
}

void NetworkManager::syncMedicineDatabase()
{
    m_currentOperation = "medicine_sync";
    emit syncStarted("正在同步药品数据库...");

    QUrl url(m_serverUrl + "/medicines/sync");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + m_authToken.toUtf8());
    }

    QJsonObject requestData;
    requestData["lastSync"] = QDateTime::currentDateTime().addDays(-7).toString(Qt::ISODate);
    requestData["clientVersion"] = "1.0.0";

    QJsonDocument doc(requestData);
    m_currentReply = m_networkManager->post(request, doc.toJson());

    connect(m_currentReply, &QNetworkReply::uploadProgress,
            this, &NetworkManager::onUploadProgress);
}

void NetworkManager::syncDiagnosisReference()
{
    m_currentOperation = "diagnosis_sync";
    emit syncStarted("正在同步诊断参考数据...");

    QUrl url(m_serverUrl + "/diagnosis/reference");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + m_authToken.toUtf8());
    }

    m_currentReply = m_networkManager->get(request);

    connect(m_currentReply, &QNetworkReply::downloadProgress,
            this, &NetworkManager::onDownloadProgress);
}

void NetworkManager::backupDatabase()
{
    m_currentOperation = "backup";
    emit syncStarted("正在备份数据库...");

    QString dbPath = "G:/Qt_file/community_medical.db";
    QFile dbFile(dbPath);

    if (!dbFile.exists()) {
        emit syncFailed("数据库文件不存在: " + dbPath);
        return;
    }

    if (!dbFile.open(QIODevice::ReadOnly)) {
        emit syncFailed("无法打开数据库文件: " + dbFile.errorString());
        return;
    }

    QUrl url(m_serverUrl + "/backup/upload");
    QNetworkRequest request(url);

    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + m_authToken.toUtf8());
    }

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-sqlite3"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"database\"; filename=\"community_medical.db\""));
    filePart.setBodyDevice(&dbFile);

    multiPart->append(filePart);

    m_currentReply = m_networkManager->post(request, multiPart);
    multiPart->setParent(m_currentReply);

    connect(m_currentReply, &QNetworkReply::uploadProgress,
            this, &NetworkManager::onUploadProgress);
}

 

void NetworkManager::downloadUpdates()
{
    m_currentOperation = "update";
    emit syncStarted("正在检查更新...");

    QUrl url(m_serverUrl + "/updates/check");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", "Bearer " + m_authToken.toUtf8());
    }

    QJsonObject requestData;
    requestData["currentVersion"] = "1.0.0";

    QJsonDocument doc(requestData);
    m_currentReply = m_networkManager->post(request, doc.toJson());
}

void NetworkManager::onReplyFinished(QNetworkReply *reply)
{
    if (reply != m_currentReply) {
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit syncFailed(reply->errorString());
        reply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject response = doc.object();

    if (response["status"].toString() != "success") {
        emit syncFailed(response["message"].toString());
        reply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    emit syncProgress(100);

    if (m_currentOperation == "medicine_sync") {
        handleMedicineSyncResponse(response);
    } else if (m_currentOperation == "diagnosis_sync") {
        handleDiagnosisResponse(response);
    } else if (m_currentOperation == "backup") {
        handleBackupResponse(response);
    } else if (m_currentOperation == "update") {
        handleUpdateResponse(response);
    }

    emit syncCompleted(response["message"].toString());
    reply->deleteLater();
    m_currentReply = nullptr;
}

void NetworkManager::handleMedicineSyncResponse(const QJsonObject &response)
{
    QJsonArray medicines = response["data"].toArray();
    QString connName = QString("medicine_sync_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName("G:/Qt_file/community_medical.db");
        if (!db.open()) {
            emit syncFailed(QString("数据库打开失败: %1").arg(db.lastError().text()));
            QSqlDatabase::removeDatabase(connName);
            return;
        }

        QSqlQuery begin(db);
        begin.exec("BEGIN");

        for (const QJsonValue &value : medicines) {
            QJsonObject medicine = value.toObject();

            QSqlQuery query(db);
            query.prepare("INSERT OR REPLACE INTO medicine (ID, MED_NAME, MED_SPEC, MED_TYPE, PRICE, STOCK, EXPIRY_DATE) "
                          "VALUES (:id, :name, :spec, :type, :price, :stock, :expiry)");
            query.bindValue(":id", medicine["id"].toString());
            query.bindValue(":name", medicine["name"].toString());
            query.bindValue(":spec", medicine["spec"].toString());
            query.bindValue(":type", medicine["type"].toString());
            query.bindValue(":price", medicine["price"].toDouble());
            query.bindValue(":stock", medicine["stock"].toInt());
            query.bindValue(":expiry", medicine["expiryDate"].toString());

            if (!query.exec()) {
                emit syncFailed(QString("药品同步失败: %1").arg(query.lastError().text()));
                QSqlQuery rollback(db);
                rollback.exec("ROLLBACK");
                db.close();
                QSqlDatabase::removeDatabase(connName);
                return;
            }
        }

        QSqlQuery commit(db);
        commit.exec("COMMIT");
        db.close();
    }
    QSqlDatabase::removeDatabase(connName);

    IDatabase &dbi = IDatabase::getInstance();
    if (!dbi.medicineTabModel) {
        if (!dbi.initMedicineModel()) {
            emit syncFailed("药品模型初始化失败");
        }
    } else {
        dbi.medicineTabModel->select();
    }
    emit medicineSynced(medicines.size());
}

void NetworkManager::handleDiagnosisResponse(const QJsonObject &response)
{
    QJsonArray diagnoses = response["data"].toArray();
    for (const QJsonValue &value : diagnoses) {
        QJsonObject d = value.toObject();
        QSqlQuery q;
        q.prepare("INSERT OR REPLACE INTO diagnosis_reference (CODE, NAME, UPDATEDTIMESTAMP) "
                  "VALUES (:code, :name, :ts)");
        q.bindValue(":code", d["code"].toString());
        q.bindValue(":name", d["name"].toString());
        q.bindValue(":ts", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        q.exec();
    }
    emit dataReceived(response);
}

void NetworkManager::handleBackupResponse(const QJsonObject &response)
{
    QString backupId = response["backupId"].toString();
    QDateTime backupTime = QDateTime::fromString(response["timestamp"].toString(), Qt::ISODate);

    // 保存备份记录到数据库
    QSqlQuery query;
    query.prepare("INSERT INTO backup_history (BACKUP_ID, TIMESTAMP, STATUS) VALUES (:id, :time, 'success')");
    query.bindValue(":id", backupId);
    query.bindValue(":time", backupTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.exec();
}

void NetworkManager::handleUpdateResponse(const QJsonObject &response)
{
    bool hasUpdate = response["hasUpdate"].toBool();
    if (hasUpdate) {
        QString version = response["latestVersion"].toString();
        QString description = response["description"].toString();

        emit dataReceived(response);
    }
}

void NetworkManager::onUploadProgress(qint64 bytesSent, qint64 totalBytes)
{
    if (totalBytes > 0) {
        int progress = static_cast<int>((bytesSent * 100) / totalBytes);
        emit syncProgress(progress);
        emit backupProgress(bytesSent, totalBytes);
    }
}

void NetworkManager::onDownloadProgress(qint64 bytesReceived, qint64 totalBytes)
{
    if (totalBytes > 0) {
        int progress = static_cast<int>((bytesReceived * 100) / totalBytes);
        emit syncProgress(progress);
    }
}
