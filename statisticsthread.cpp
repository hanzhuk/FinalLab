#include "statisticsthread.h"
#include "idatabase.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

StatisticsThread::StatisticsThread(QObject *parent)
    : QThread(parent)
{
}

void StatisticsThread::setDateRange(const QDate &start, const QDate &end)
{
    m_startDate = start;
    m_endDate = end;
}

void StatisticsThread::run()
{
    emit progressUpdated(10);

    QString connectionName = QString("statistics_connection_%1")
                                 .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("G:/Qt_file/community_medical.db");

        if (!db.open()) {
            emit statisticsFailed(QString("统计线程数据库打开失败: %1").arg(db.lastError().text()));
            return;
        }

        QJsonObject result;

        try {
            if (m_reportType == "patient") {
                result = generatePatientReport(db);
            } else if (m_reportType == "medicine") {
                result = generateMedicineStockReport(db);
            } else if (m_reportType == "doctor") {
                result = generateDoctorWorkloadReport(db);
            } else if (m_reportType == "finance") {
                result = generateFinancialReport(db);
            } else {
                emit statisticsFailed("未知的报表类型");
                db.close();
                return;
            }

            emit progressUpdated(100);
            emit statisticsCompleted(result);

        } catch (const std::exception &e) {
            emit statisticsFailed(QString("统计异常: %1").arg(e.what()));
        }

        db.close();
    }

    QSqlDatabase::removeDatabase(connectionName);
}

QJsonObject StatisticsThread::generatePatientReport(QSqlDatabase &db)
{
    QJsonObject report;
    QJsonArray dataArray;

    QString startDateStr = m_startDate.toString("yyyy-MM-dd");
    QString endDateStr = m_endDate.toString("yyyy-MM-dd");

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) as total, SEX, "
                  "COUNT(CASE WHEN DOB >= date('now', '-18 years') THEN 1 END) as minors, "
                  "COUNT(CASE WHEN DOB < date('now', '-60 years') THEN 1 END) as seniors "
                  "FROM patient GROUP BY SEX");

    if (!query.exec()) {
        emit statisticsFailed(query.lastError().text());
        return report;
    }

    emit progressUpdated(30);

    while (query.next()) {
        QJsonObject item;
        item["sex"] = query.value("SEX").toString();
        item["total"] = query.value("total").toInt();
        item["minors"] = query.value("minors").toInt();
        item["seniors"] = query.value("seniors").toInt();
        dataArray.append(item);
    }

    report["type"] = "patient";
    report["title"] = "患者统计报告";
    report["data"] = dataArray;
    report["dateRange"] = QString("%1 至 %2").arg(startDateStr).arg(endDateStr);

    emit progressUpdated(70);
    return report;
}

QJsonObject StatisticsThread::generateMedicineStockReport(QSqlDatabase &db)
{
    QJsonObject report;
    QJsonArray dataArray;

    QSqlQuery query(db);
    query.prepare("SELECT MED_NAME, STOCK, EXPIRY_DATE, MED_TYPE FROM medicine "
                  "WHERE STOCK < 100 OR EXPIRY_DATE <= date('now', '+30 days')");

    if (!query.exec()) {
        emit statisticsFailed(query.lastError().text());
        return report;
    }

    emit progressUpdated(30);

    int lowStockCount = 0;
    int nearExpiryCount = 0;

    while (query.next()) {
        QJsonObject item;
        item["name"] = query.value("MED_NAME").toString();
        item["stock"] = query.value("STOCK").toInt();
        item["expiry"] = query.value("EXPIRY_DATE").toString();
        item["type"] = query.value("MED_TYPE").toString();

        if (query.value("STOCK").toInt() < 100) {
            lowStockCount++;
            item["alertType"] = "库存不足";
        }
        if (query.value("EXPIRY_DATE").toDate() <= QDate::currentDate().addDays(30)) {
            nearExpiryCount++;
            item["alertType"] = "即将过期";
        }

        dataArray.append(item);
    }

    report["type"] = "medicine";
    report["title"] = "药品预警报告";
    report["data"] = dataArray;
    report["lowStockCount"] = lowStockCount;
    report["nearExpiryCount"] = nearExpiryCount;

    emit progressUpdated(70);
    return report;
}

QJsonObject StatisticsThread::generateDoctorWorkloadReport(QSqlDatabase &db)
{
    QJsonObject report;
    QJsonArray dataArray;

    QSqlQuery query(db);
    query.prepare("SELECT d.NAME, d.TITLE, COUNT(mr.ID) as visitCount, "
                  "AVG(julianday(mr.VISIT_DATE) - julianday(mr.CREATEDTIMESTAMP)) as avgDuration "
                  "FROM doctor d "
                  "LEFT JOIN medical_record mr ON d.ID = mr.DOCTOR_ID "
                  "WHERE mr.VISIT_DATE >= date('now', '-30 days') "
                  "GROUP BY d.ID");

    if (!query.exec()) {
        emit statisticsFailed(query.lastError().text());
        return report;
    }

    emit progressUpdated(30);

    while (query.next()) {
        QJsonObject item;
        item["name"] = query.value("NAME").toString();
        item["title"] = query.value("TITLE").toString();
        item["visitCount"] = query.value("visitCount").toInt();
        item["avgDuration"] = query.value("avgDuration").toDouble();
        dataArray.append(item);
    }

    report["type"] = "doctor";
    report["title"] = "医生工作量统计";
    report["data"] = dataArray;

    emit progressUpdated(70);
    return report;
}

QJsonObject StatisticsThread::generateFinancialReport(QSqlDatabase &db)
{
    QJsonObject report;
    QJsonArray dataArray;

    QSqlQuery query(db);
    query.prepare("SELECT strftime('%Y-%m', mr.VISIT_DATE) as month, "
                  "COUNT(DISTINCT mr.ID) as visitCount, "
                  "SUM(m.PRICE) as totalRevenue "
                  "FROM medical_record mr "
                  "JOIN prescription p ON mr.ID = p.RECORD_ID "
                  "JOIN medicine m ON p.MEDICINE_ID = m.ID "
                  "WHERE mr.VISIT_DATE >= date('now', '-12 months') "
                  "GROUP BY month");

    if (!query.exec()) {
        emit statisticsFailed(query.lastError().text());
        return report;
    }

    emit progressUpdated(30);

    while (query.next()) {
        QJsonObject item;
        item["month"] = query.value("month").toString();
        item["visitCount"] = query.value("visitCount").toInt();
        item["revenue"] = query.value("totalRevenue").toDouble();
        dataArray.append(item);
    }

    report["type"] = "finance";
    report["title"] = "财务收入统计";
    report["data"] = dataArray;

    emit progressUpdated(70);
    return report;
}
