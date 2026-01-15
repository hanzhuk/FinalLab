#include "medicalrecordeditview.h"
#include "ui_medicalrecordeditview.h"
#include "idatabase.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QMessageBox>

MedicalRecordEditView::MedicalRecordEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::MedicalRecordEditView)
{
    Q_UNUSED(index);
    ui->setupUi(this);
    ui->editVisitDate->setDateTime(QDateTime::currentDateTime());

    QString connectionName = QString("mr_edit_fill_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("G:/Qt_file/community_medical.db");
        if (db.open()) {
            {
                QSqlQuery q(db);
                q.exec("SELECT ID, NAME FROM patient ORDER BY NAME ASC");
                while (q.next()) {
                    ui->editPatient->addItem(q.value(1).toString(), q.value(0).toString());
                }
            }
            {
                QSqlQuery q(db);
                q.exec("SELECT ID, NAME FROM doctor ORDER BY NAME ASC");
                while (q.next()) {
                    ui->editDoctor->addItem(q.value(1).toString(), q.value(0).toString());
                }
            }
            {
                QSqlQuery q(db);
                q.exec("SELECT ID, DEPT_NAME FROM department ORDER BY DEPT_NAME ASC");
                while (q.next()) {
                    ui->editDept->addItem(q.value(1).toString(), q.value(0).toString());
                }
            }
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(connectionName);
}

MedicalRecordEditView::~MedicalRecordEditView()
{
    delete ui;
}

void MedicalRecordEditView::on_btSave_clicked()
{
    QString newId = IDatabase::generateUUID();

    QString connectionName = QString("mr_edit_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName("G:/Qt_file/community_medical.db");

        if (!db.open()) {
            QMessageBox::critical(this, "错误", "数据库打开失败：" + db.lastError().text());
            QSqlDatabase::removeDatabase(connectionName);
            return;
        }

        QString patientId = ui->editPatient->currentData().toString();
        QString doctorId  = ui->editDoctor->currentData().toString();
        QString deptId    = ui->editDept->currentData().toString();

        if (patientId.isEmpty() || doctorId.isEmpty() || deptId.isEmpty()) {
            QMessageBox::critical(this, "错误", "患者/医生/科室名称未找到，请先在对应模块创建或选择正确名称");
            db.close();
            QSqlDatabase::removeDatabase(connectionName);
            return;
        }

        QSqlQuery query(db);
        query.prepare("INSERT INTO medical_record "
                      "(ID, PATIENT_ID, DOCTOR_ID, DEPT_ID, DIAGNOSIS, VISIT_DATE, CREATEDTIMESTAMP) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(newId);
        query.addBindValue(patientId);
        query.addBindValue(doctorId);
        query.addBindValue(deptId);
        query.addBindValue(ui->editDiagnosis->text());
        query.addBindValue(ui->editVisitDate->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "插入就诊记录失败：" + query.lastError().text());
            db.close();
            QSqlDatabase::removeDatabase(connectionName);
            return;
        }

        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);

    IDatabase::getInstance().updateRecordView();
    QMessageBox::information(this, "成功", "就诊记录添加成功");
    emit goPreviousView();
}

void MedicalRecordEditView::on_btCancel_clicked()
{
    emit goPreviousView();
}
