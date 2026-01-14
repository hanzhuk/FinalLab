#ifndef STATISTICSTHREAD_H
#define STATISTICSTHREAD_H

#include <QObject>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlDatabase>

class StatisticsThread : public QThread
{
    Q_OBJECT

public:
    explicit StatisticsThread(QObject *parent = nullptr);

    void setReportType(const QString &type) { m_reportType = type; }
    void setDateRange(const QDate &start, const QDate &end);

signals:
    void statisticsCompleted(QJsonObject result);
    void statisticsFailed(QString error);
    void progressUpdated(int percentage);

protected:
    void run() override;

private:
    QString m_reportType;
    QDate m_startDate;
    QDate m_endDate;

    QJsonObject generatePatientReport(QSqlDatabase &db);
    QJsonObject generateMedicineStockReport(QSqlDatabase &db);
    QJsonObject generateDoctorWorkloadReport(QSqlDatabase &db);
    QJsonObject generateFinancialReport(QSqlDatabase &db);
};

#endif // STATISTICSTHREAD_H
