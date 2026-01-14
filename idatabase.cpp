#include "idatabase.h"
#include <QUuid>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QtConcurrent>

IDatabase::IDatabase(QObject *parent) : QObject{parent}
{
    ininDatabase();
    initializeTables();
}

IDatabase::~IDatabase()
{
    if (database.isOpen()) {
        database.close();
    }
}

void IDatabase::ininDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");

    QString aFile = "G:/Qt_file/community_medical.db";

    database.setDatabaseName(aFile);

    QFileInfo fileInfo(aFile);
    QDir dir;
    dir.mkpath(fileInfo.absolutePath());

    if (!database.open()) {
        qDebug() << "❌ 数据库打开失败：" << database.lastError().text();
        qDebug() << "📂 尝试的路径：" << aFile;
    } else {
        qDebug() << "✅ 数据库连接成功";
        qDebug() << "📊 已存在的表：" << database.tables();

        QStringList requiredTables = {"user", "patient", "doctor", "department", "medicine"};
        QStringList existingTables = database.tables();
        for (const QString &table : requiredTables) {
            if (!existingTables.contains(table, Qt::CaseInsensitive)) {
                qDebug() << "⚠️ 警告：核心表" << table << "不存在！";
            }
        }
    }
}

void IDatabase::initializeTables()
{
    QMutexLocker locker(&databaseMutex);
    qDebug() << "数据库初始化检查完成";
}

QString IDatabase::userLogin(QString userName, QString password)
{
    QMutexLocker locker(&databaseMutex);

    QSqlQuery query;
    query.prepare("SELECT password FROM user WHERE username = :USER");
    query.bindValue(":USER", userName);

    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        return "databaseError";
    }

    if (query.next()) {
        QString dbPassword = query.value("password").toString();
        if (dbPassword == password) {
            qDebug() << "✅ 登录成功：" << userName;
            return "loginOk";
        } else {
            qDebug() << "❌ 密码错误：" << userName;
            return "wrongPassword";
        }
    } else {
        qDebug() << "❌ 用户不存在：" << userName;
        return "wrongUsername";
    }
}

bool IDatabase::initPatientModel()
{
    QMutexLocker locker(&databaseMutex);

    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

    if (!patientTabModel->select()) {
        qDebug() << "❌ 患者模型初始化失败：" << patientTabModel->lastError().text();
        return false;
    }

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    QMutexLocker locker(&databaseMutex);

    int row = patientTabModel->rowCount();
    patientTabModel->insertRow(row);

    QModelIndex curIndex = patientTabModel->index(row, patientTabModel->fieldIndex("ID"));
    QString newId = generateUUID();
    patientTabModel->setData(curIndex, newId);

    curIndex = patientTabModel->index(row, patientTabModel->fieldIndex("CREATEDTIMESTAMP"));
    patientTabModel->setData(curIndex, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return row;
}

bool IDatabase::searchPatient(QString filter)
{
    QMutexLocker locker(&databaseMutex);
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QMutexLocker locker(&databaseMutex);

    QModelIndex curIndex = thePatientSelection->currentIndex();
    if (!curIndex.isValid()) return false;

    patientTabModel->removeRow(curIndex.row());
    bool success = patientTabModel->submitAll();
    if (success) {
        emit patientDataChanged();
    }
    return success;
}

bool IDatabase::submitPatientEdit()
{
    QMutexLocker locker(&databaseMutex);
    bool success = patientTabModel->submitAll();
    if (success) {
        emit patientDataChanged();
    }
    return success;
}

void IDatabase::revertPatientEdit()
{
    QMutexLocker locker(&databaseMutex);
    patientTabModel->revertAll();
}

bool IDatabase::initDoctorModel()
{
    QMutexLocker locker(&databaseMutex);

    doctorTabModel = new QSqlTableModel(this, database);
    doctorTabModel->setTable("doctor");
    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    doctorTabModel->setSort(doctorTabModel->fieldIndex("NAME"), Qt::AscendingOrder);

    if (!doctorTabModel->select()) {
        qDebug() << "❌ 医生模型初始化失败：" << doctorTabModel->lastError().text();
        return false;
    }

    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

int IDatabase::addNewDoctor()
{
    QMutexLocker locker(&databaseMutex);

    int row = doctorTabModel->rowCount();
    doctorTabModel->insertRow(row);

    QModelIndex curIndex = doctorTabModel->index(row, doctorTabModel->fieldIndex("ID"));
    doctorTabModel->setData(curIndex, generateUUID());

    curIndex = doctorTabModel->index(row, doctorTabModel->fieldIndex("CREATEDTIMESTAMP"));
    doctorTabModel->setData(curIndex, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return row;
}

bool IDatabase::searchDoctor(QString filter)
{
    QMutexLocker locker(&databaseMutex);
    doctorTabModel->setFilter(filter);
    return doctorTabModel->select();
}

bool IDatabase::deleteCurrentDoctor()
{
    QMutexLocker locker(&databaseMutex);

    QModelIndex curIndex = theDoctorSelection->currentIndex();
    if (!curIndex.isValid()) return false;

    doctorTabModel->removeRow(curIndex.row());
    bool success = doctorTabModel->submitAll();
    if (success) {
        emit doctorDataChanged();
    }
    return success;
}

bool IDatabase::submitDoctorEdit()
{
    QMutexLocker locker(&databaseMutex);
    bool success = doctorTabModel->submitAll();
    if (success) {
        emit doctorDataChanged();
    }
    return success;
}

void IDatabase::revertDoctorEdit()
{
    QMutexLocker locker(&databaseMutex);
    doctorTabModel->revertAll();
}

bool IDatabase::initDepartmentModel()
{
    QMutexLocker locker(&databaseMutex);

    departmentTabModel = new QSqlTableModel(this, database);
    departmentTabModel->setTable("department");
    departmentTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    departmentTabModel->setSort(departmentTabModel->fieldIndex("DEPT_NAME"), Qt::AscendingOrder);

    if (!departmentTabModel->select()) {
        qDebug() << "❌ 科室模型初始化失败：" << departmentTabModel->lastError().text();
        return false;
    }

    theDepartmentSelection = new QItemSelectionModel(departmentTabModel);
    return true;
}

int IDatabase::addNewDepartment()
{
    QMutexLocker locker(&databaseMutex);

    int row = departmentTabModel->rowCount();
    departmentTabModel->insertRow(row);

    QModelIndex curIndex = departmentTabModel->index(row, departmentTabModel->fieldIndex("ID"));
    departmentTabModel->setData(curIndex, generateUUID());

    curIndex = departmentTabModel->index(row, departmentTabModel->fieldIndex("CREATEDTIMESTAMP"));
    departmentTabModel->setData(curIndex, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return row;
}

bool IDatabase::searchDepartment(QString filter)
{
    QMutexLocker locker(&databaseMutex);
    departmentTabModel->setFilter(filter);
    return departmentTabModel->select();
}

bool IDatabase::deleteCurrentDepartment()
{
    QMutexLocker locker(&databaseMutex);

    QModelIndex curIndex = theDepartmentSelection->currentIndex();
    if (!curIndex.isValid()) return false;

    departmentTabModel->removeRow(curIndex.row());
    bool success = departmentTabModel->submitAll();
    if (success) {
        emit departmentDataChanged();
    }
    return success;
}

bool IDatabase::submitDepartmentEdit()
{
    QMutexLocker locker(&databaseMutex);
    bool success = departmentTabModel->submitAll();
    if (success) {
        emit departmentDataChanged();
    }
    return success;
}

void IDatabase::revertDepartmentEdit()
{
    QMutexLocker locker(&databaseMutex);
    departmentTabModel->revertAll();
}

bool IDatabase::initMedicineModel()
{
    QMutexLocker locker(&databaseMutex);

    medicineTabModel = new QSqlTableModel(this, database);
    medicineTabModel->setTable("medicine");
    medicineTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    medicineTabModel->setSort(medicineTabModel->fieldIndex("MED_NAME"), Qt::AscendingOrder);

    if (!medicineTabModel->select()) {
        qDebug() << "❌ 药品模型初始化失败：" << medicineTabModel->lastError().text();
        return false;
    }

    theMedicineSelection = new QItemSelectionModel(medicineTabModel);
    return true;
}

int IDatabase::addNewMedicine()
{
    QMutexLocker locker(&databaseMutex);

    int row = medicineTabModel->rowCount();
    medicineTabModel->insertRow(row);

    QModelIndex curIndex = medicineTabModel->index(row, medicineTabModel->fieldIndex("ID"));
    medicineTabModel->setData(curIndex, generateUUID());

    curIndex = medicineTabModel->index(row, medicineTabModel->fieldIndex("CREATEDTIMESTAMP"));
    medicineTabModel->setData(curIndex, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return row;
}

bool IDatabase::searchMedicine(QString filter)
{
    QMutexLocker locker(&databaseMutex);
    medicineTabModel->setFilter(filter);
    return medicineTabModel->select();
}

bool IDatabase::deleteCurrentMedicine()
{
    QMutexLocker locker(&databaseMutex);

    QModelIndex curIndex = theMedicineSelection->currentIndex();
    if (!curIndex.isValid()) return false;

    medicineTabModel->removeRow(curIndex.row());
    bool success = medicineTabModel->submitAll();
    if (success) {
        emit medicineDataChanged();
    }
    return success;
}

bool IDatabase::submitMedicineEdit()
{
    QMutexLocker locker(&databaseMutex);
    bool success = medicineTabModel->submitAll();
    if (success) {
        emit medicineDataChanged();
    }
    return success;
}

void IDatabase::revertMedicineEdit()
{
    QMutexLocker locker(&databaseMutex);
    medicineTabModel->revertAll();
}

bool IDatabase::initMedicalRecordModel()
{
    if (!recordTabModel) {
        recordTabModel = new QStandardItemModel(this);
    }
    // 注意：不再这里直接加载数据，改为在 loadMedicalRecordsAsync 中加载

    if (!theRecordSelection) {
        theRecordSelection = new QItemSelectionModel(recordTabModel);
    }
    return true;
}

void IDatabase::updateRecordView()
{
    // 调用异步加载
    loadMedicalRecordsAsync();
}

bool IDatabase::initAppointmentModel()
{
    if (!appointmentTabModel) {
        appointmentTabModel = new QStandardItemModel(this);
    }

    if (!theAppointmentSelection) {
        theAppointmentSelection = new QItemSelectionModel(appointmentTabModel);
    }
    return true;
}

void IDatabase::updateAppointmentView()
{
    loadAppointmentsAsync();
}

QString IDatabase::generateUUID()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}


// 异步加载实现
void IDatabase::loadMedicalRecordsAsync()
{
    // 确保模型已初始化
    if (!recordTabModel) initMedicalRecordModel();

    QtConcurrent::run([this]() {
        QString connectionName = QString("worker_loader_mr_%1").arg((quintptr)QThread::currentThreadId());
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
            db.setDatabaseName("G:/Qt_file/community_medical.db");

            if (db.open()) {
                QSqlQuery query(db);
                // 恢复包含 JOIN 的完整查询，因为现在是在后台线程运行，不会阻塞 UI
                query.prepare("SELECT mr.ID, p.NAME as PATIENT_NAME, d.NAME as DOCTOR_NAME, "
                              "dept.DEPT_NAME as DEPT_NAME, mr.DIAGNOSIS, mr.VISIT_DATE "
                              "FROM medical_record mr "
                              "LEFT JOIN patient p ON mr.PATIENT_ID = p.ID "
                              "LEFT JOIN doctor d ON mr.DOCTOR_ID = d.ID "
                              "LEFT JOIN department dept ON mr.DEPT_ID = dept.ID "
                              "ORDER BY mr.VISIT_DATE DESC");

                if (query.exec()) {
                    QList<QVariantMap> results;
                    while (query.next()) {
                        QVariantMap row;
                        row["ID"] = query.value("ID");
                        row["PATIENT_NAME"] = query.value("PATIENT_NAME");
                        row["DOCTOR_NAME"] = query.value("DOCTOR_NAME");
                        row["DEPT_NAME"] = query.value("DEPT_NAME");
                        row["DIAGNOSIS"] = query.value("DIAGNOSIS");
                        row["VISIT_DATE"] = query.value("VISIT_DATE");
                        results.append(row);
                    }

                    // 回到主线程更新 UI
                    QMetaObject::invokeMethod(this, [this, results]() {
                        recordTabModel->clear();
                        recordTabModel->setHorizontalHeaderLabels({"ID", "患者姓名", "医生姓名", "科室", "诊断", "就诊时间"});

                        for (const auto &row : results) {
                            QList<QStandardItem*> items;
                            items << new QStandardItem(row["ID"].toString());
                            items << new QStandardItem(row["PATIENT_NAME"].toString());
                            items << new QStandardItem(row["DOCTOR_NAME"].toString());
                            items << new QStandardItem(row["DEPT_NAME"].toString());
                            items << new QStandardItem(row["DIAGNOSIS"].toString());
                            items << new QStandardItem(row["VISIT_DATE"].toString());
                            recordTabModel->appendRow(items);
                        }

                        // 重新关联 SelectionModel，因为 clear() 可能重置了模型状态
                        if (theRecordSelection) {
                            delete theRecordSelection;
                        }
                        theRecordSelection = new QItemSelectionModel(recordTabModel);

                        emit medicalRecordLoaded();
                    });
                } else {
                    qDebug() << "❌ 后台加载就诊记录失败：" << query.lastError().text();
                }
            } else {
                qDebug() << "❌ 后台数据库打开失败";
            }
            db.close();
        }
        QSqlDatabase::removeDatabase(connectionName);
    });
}

void IDatabase::loadAppointmentsAsync()
{
    // 确保模型已初始化
    if (!appointmentTabModel) initAppointmentModel();

    QtConcurrent::run([this]() {
        QString connectionName = QString("worker_loader_app_%1").arg((quintptr)QThread::currentThreadId());
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
            db.setDatabaseName("G:/Qt_file/community_medical.db");

            if (db.open()) {
                QSqlQuery query(db);
                query.prepare("SELECT a.ID, p.NAME as PATIENT_NAME, d.NAME as DOCTOR_NAME, "
                              "dept.DEPT_NAME as DEPT_NAME, a.APPOINT_TIME, a.STATUS "
                              "FROM appointment a "
                              "LEFT JOIN patient p ON a.PATIENT_ID = p.ID "
                              "LEFT JOIN doctor d ON a.DOCTOR_ID = d.ID "
                              "LEFT JOIN department dept ON a.DEPT_ID = dept.ID "
                              "ORDER BY a.APPOINT_TIME DESC");

                if (query.exec()) {
                    QList<QVariantMap> results;
                    while (query.next()) {
                        QVariantMap row;
                        row["ID"] = query.value("ID");
                        row["PATIENT_NAME"] = query.value("PATIENT_NAME");
                        row["DOCTOR_NAME"] = query.value("DOCTOR_NAME");
                        row["DEPT_NAME"] = query.value("DEPT_NAME");
                        row["APPOINT_TIME"] = query.value("APPOINT_TIME");
                        row["STATUS"] = query.value("STATUS");
                        results.append(row);
                    }

                    // 回到主线程更新 UI
                    QMetaObject::invokeMethod(this, [this, results]() {
                        appointmentTabModel->clear();
                        appointmentTabModel->setHorizontalHeaderLabels({"ID", "患者姓名", "医生姓名", "科室", "预约时间", "状态"});

                        for (const auto &row : results) {
                            QList<QStandardItem*> items;
                            items << new QStandardItem(row["ID"].toString());
                            items << new QStandardItem(row["PATIENT_NAME"].toString());
                            items << new QStandardItem(row["DOCTOR_NAME"].toString());
                            items << new QStandardItem(row["DEPT_NAME"].toString());
                            items << new QStandardItem(row["APPOINT_TIME"].toString());
                            items << new QStandardItem(row["STATUS"].toString());
                            appointmentTabModel->appendRow(items);
                        }

                        if (theAppointmentSelection) {
                            delete theAppointmentSelection;
                        }
                        theAppointmentSelection = new QItemSelectionModel(appointmentTabModel);

                        emit appointmentLoaded();
                    });
                } else {
                    qDebug() << "❌ 后台加载预约失败：" << query.lastError().text();
                }
            } else {
                qDebug() << "❌ 后台数据库打开失败";
            }
            db.close();
        }
        QSqlDatabase::removeDatabase(connectionName);
    });
}

// 统计方法实现
QJsonObject IDatabase::getPatientStatistics(const QDate &start, const QDate &end)
{
    QJsonObject result;
    QSqlQuery query(database);

    query.prepare("SELECT COUNT(*) as total FROM patient WHERE CREATEDTIMESTAMP BETWEEN :start AND :end");
    query.bindValue(":start", start.toString("yyyy-MM-dd"));
    query.bindValue(":end", end.toString("yyyy-MM-dd"));

    if (query.exec() && query.next()) {
        result["totalPatients"] = query.value("total").toInt();
    }

    return result;
}

QJsonObject IDatabase::getMedicineWarningStatistics()
{
    return QJsonObject(); // Placeholder
}

QJsonObject IDatabase::getDoctorWorkloadStatistics(const QDate &start, const QDate &end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    return QJsonObject(); // Placeholder
}

QJsonObject IDatabase::getFinancialStatistics(const QDate &start, const QDate &end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    return QJsonObject(); // Placeholder
}

// 注意：addNewMedicalRecord 实现需要适配，这里只保留声明，具体实现看 user 之前的代码，如果没用到可以忽略
int IDatabase::addNewMedicalRecord()
{
    return 0; // Placeholder
}
