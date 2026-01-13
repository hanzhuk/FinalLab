#ifndef STATISTICSTHREAD_H
#define STATISTICSTHREAD_H

#include <QObject>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>

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

    QJsonObject generatePatientReport();
    QJsonObject generateMedicineStockReport();
    QJsonObject generateDoctorWorkloadReport();
    QJsonObject generateFinancialReport();
};

#endif // STATISTICSTHREAD_H
