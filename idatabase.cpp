#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database=QSqlDatabase::addDatabase("QSQLITE");
    QString aFile="G:/Qt_file/community_medical.db";
    database.setDatabaseName(aFile);

    if (!database.open()) { //打开数据库
        qDebug() << "failed to open database";
    } else
        qDebug() << "open database is ok" << database.connectionName();

}

bool IDatabase::initDoctorModel()
{
    doctorTabModel = new QSqlTableModel(this, database);
    doctorTabModel->setTable("doctor");
    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    doctorTabModel->setSort(doctorTabModel->fieldIndex("NAME"), Qt::AscendingOrder);
    if (!doctorTabModel->select())
        return false;

    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

bool IDatabase::initDepartmentModel()
{
    departmentTabModel = new QSqlTableModel(this, database);
    departmentTabModel->setTable("department");
    departmentTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    departmentTabModel->setSort(departmentTabModel->fieldIndex("DEPT_NAME"), Qt::AscendingOrder);
    if (!departmentTabModel->select())
        return false;

    theDepartmentSelection = new QItemSelectionModel(departmentTabModel);
    return true;
}

bool IDatabase::initMedicineModel()
{
    medicineTabModel = new QSqlTableModel(this, database);
    medicineTabModel->setTable("medicine");
    medicineTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    medicineTabModel->setSort(medicineTabModel->fieldIndex("MED_NAME"), Qt::AscendingOrder);
    if (!medicineTabModel->select())
        return false;

    theMedicineSelection = new QItemSelectionModel(medicineTabModel);
    return true;
}

bool IDatabase::initMedicalRecordModel()
{

    recordTabModel = new QSqlQueryModel(this);
    updateRecordView();
    return true;
}

bool IDatabase::initAppointmentModel()
{
    appointmentTabModel = new QSqlQueryModel(this);
    updateAppointmentView();
    return true;
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit); //数据保存方式，OnManualSubmit , OnRowChange
    patientTabModel->setSort(patientTabModel->fieldIndex("name"), Qt::AscendingOrder); //排序
    if (!(patientTabModel->select()))//查询数据
        return false;

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;

}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(), QModelIndex());

    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));

    patientTabModel->setRecord(curRecNo, curRec);


    return curIndex.row();
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex(); // 获取当前选择单元格的模型索引

    if (!curIndex.isValid()) {
        return false;
    }
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();
    return true; // 补上返回值
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString password)
{
    //   return "loginOk";
    QSqlQuery query;
    query.prepare("select username, password from user where username = :USER");
    query.bindValue(":USER", userName);
    query.exec();
    qDebug() << query.lastQuery() << query.first();

    if (query.first() && query.value("username").isValid()) {
        QString passwd = query.value("password").toString();
        if (passwd == password) {
            qDebug() << "login ok";
            return "loginOk";
        } else {
            qDebug() << "wrong password";
            return "wrongPassword";
        }
    } else {
        qDebug() << "no such user";
        return "wrongUsername";
    }
}

IDatabase::IDatabase(QObject *parent)   : QObject{parent}
{
    ininDatabase();
}
