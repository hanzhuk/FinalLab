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
    // ==================== 单例模式 ====================
    static IDatabase &getInstance()
    {
        static IDatabase instance;
        return instance;
    }

    // ==================== 用户管理 ====================
    QString userLogin(QString userName, QString password);

    // ==================== 患者管理 ====================
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void revertPatientEdit();

    // ==================== 医生管理 ====================
    bool initDoctorModel();
    int addNewDoctor();
    bool searchDoctor(QString filter);
    bool deleteCurrentDoctor();
    bool submitDoctorEdit();
    void revertDoctorEdit();

    // ==================== 科室管理 ====================
    bool initDepartmentModel();
    int addNewDepartment();
    bool searchDepartment(QString filter);
    bool deleteCurrentDepartment();
    bool submitDepartmentEdit();
    void revertDepartmentEdit();

    // ==================== 药品管理 ====================
    bool initMedicineModel();
    int addNewMedicine();
    bool searchMedicine(QString filter);
    bool deleteCurrentMedicine();
    bool submitMedicineEdit();
    void revertMedicineEdit();

    // ==================== 就诊记录 ====================
    bool initMedicalRecordModel();
    void updateRecordView(); // 刷新视图
    bool addNewMedicalRecord();

    // ==================== 预约管理 ====================
    bool initAppointmentModel();
    void updateAppointmentView(); // 刷新视图

    // ==================== 数据模型（供 View 使用）====================
    QSqlTableModel *patientTabModel = nullptr;
    QItemSelectionModel *thePatientSelection = nullptr;

    QSqlTableModel *doctorTabModel = nullptr;
    QItemSelectionModel *theDoctorSelection = nullptr;

    QSqlTableModel *departmentTabModel = nullptr;
    QItemSelectionModel *theDepartmentSelection = nullptr;

    QSqlTableModel *medicineTabModel = nullptr;
    QItemSelectionModel *theMedicineSelection = nullptr;

    QSqlQueryModel *recordTabModel = nullptr;  // 注意：就诊记录用 QueryModel
    QItemSelectionModel *theRecordSelection = nullptr;

    QSqlQueryModel *appointmentTabModel = nullptr;  // 预约也用 QueryModel
    QItemSelectionModel *theAppointmentSelection = nullptr;

signals:
    // ==================== 数据变更通知 ====================
    void patientDataChanged();
    void doctorDataChanged();
    void departmentDataChanged();
    void medicineDataChanged();
    void recordDataChanged();
    void appointmentDataChanged();

private:
    // ==================== 构造函数与数据库连接 ====================
    explicit IDatabase(QObject *parent = nullptr);
    ~IDatabase();

    // 禁止拷贝
    IDatabase(IDatabase const &) = delete;
    void operator=(IDatabase const &) = delete;

    // ==================== 数据库实例 ====================
    QSqlDatabase database;
    QMutex databaseMutex; // 线程安全保护

    // ==================== 初始化方法 ====================
    void ininDatabase();
    void initializeTables(); // 初始化所有表结构

    // ==================== 通用辅助方法 ====================
    QString generateUUID();
};

#endif // IDATABASE_H
