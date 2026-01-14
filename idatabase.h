#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>
#include <QMutex>
#include <QJsonObject>
#include <QDate>

#include <QStandardItemModel>

class IDatabase : public QObject
{
    Q_OBJECT

public:
    static IDatabase &getInstance()
    {
        static IDatabase instance;
        return instance;
    }

    QString userLogin(QString userName, QString password);
    static QString generateUUID();

    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void revertPatientEdit();

    bool initDoctorModel();
    int addNewDoctor();
    bool searchDoctor(QString filter);
    bool deleteCurrentDoctor();
    bool submitDoctorEdit();
    void revertDoctorEdit();

    bool initDepartmentModel();
    int addNewDepartment();
    bool searchDepartment(QString filter);
    bool deleteCurrentDepartment();
    bool submitDepartmentEdit();
    void revertDepartmentEdit();

    bool initMedicineModel();
    int addNewMedicine();
    bool searchMedicine(QString filter);
    bool deleteCurrentMedicine();
    bool submitMedicineEdit();
    void revertMedicineEdit();

    bool initMedicalRecordModel();
    void updateRecordView();
    //bool addNewMedicalRecord();
    int addNewMedicalRecord();

    bool initAppointmentModel();
    void updateAppointmentView();

    QJsonObject getPatientStatistics(const QDate &start, const QDate &end);
    QJsonObject getMedicineWarningStatistics();
    QJsonObject getDoctorWorkloadStatistics(const QDate &start, const QDate &end);
    QJsonObject getFinancialStatistics(const QDate &start, const QDate &end);

    QSqlTableModel *patientTabModel = nullptr;
    QItemSelectionModel *thePatientSelection = nullptr;

    QSqlTableModel *doctorTabModel = nullptr;
    QItemSelectionModel *theDoctorSelection = nullptr;

    QSqlTableModel *departmentTabModel = nullptr;
    QItemSelectionModel *theDepartmentSelection = nullptr;

    QSqlTableModel *medicineTabModel = nullptr;
    QItemSelectionModel *theMedicineSelection = nullptr;

    QStandardItemModel *recordTabModel = nullptr;
    QItemSelectionModel *theRecordSelection = nullptr;

    QStandardItemModel *appointmentTabModel = nullptr;
    QItemSelectionModel *theAppointmentSelection = nullptr;

signals:
    void patientDataChanged();
    void doctorDataChanged();
    void departmentDataChanged();
    void medicineDataChanged();
    void recordDataChanged();
    void appointmentDataChanged();
    void medicalRecordLoaded();     // 就诊记录加载完成信号
    void appointmentLoaded();       // 预约加载完成信号

public slots:
    void loadMedicalRecordsAsync(); // 异步加载就诊记录
    void loadAppointmentsAsync();   // 异步加载预约

private:
    explicit IDatabase(QObject *parent = nullptr);
    ~IDatabase();

    IDatabase(IDatabase const &) = delete;
    void operator=(IDatabase const &)  = delete;

    QSqlDatabase database;
    QMutex databaseMutex;

    void ininDatabase();
    void initializeTables();

    // QString generateUUID();
};

#endif
