#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>
#include <QMutex>

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
    bool addNewMedicalRecord();

    bool initAppointmentModel();
    void updateAppointmentView();

    QSqlTableModel *patientTabModel = nullptr;
    QItemSelectionModel *thePatientSelection = nullptr;

    QSqlTableModel *doctorTabModel = nullptr;
    QItemSelectionModel *theDoctorSelection = nullptr;

    QSqlTableModel *departmentTabModel = nullptr;
    QItemSelectionModel *theDepartmentSelection = nullptr;

    QSqlTableModel *medicineTabModel = nullptr;
    QItemSelectionModel *theMedicineSelection = nullptr;

    QSqlQueryModel *recordTabModel = nullptr;
    QItemSelectionModel *theRecordSelection = nullptr;

    QSqlQueryModel *appointmentTabModel = nullptr;
    QItemSelectionModel *theAppointmentSelection = nullptr;

signals:
    void patientDataChanged();
    void doctorDataChanged();
    void departmentDataChanged();
    void medicineDataChanged();
    void recordDataChanged();
    void appointmentDataChanged();

private:
    explicit IDatabase(QObject *parent = nullptr);
    ~IDatabase();

    IDatabase(IDatabase const &) = delete;
    void operator=(IDatabase const &) = delete;

    QSqlDatabase database;
    QMutex databaseMutex;

    void ininDatabase();
    void initializeTables();

    QString generateUUID();
};

#endif
