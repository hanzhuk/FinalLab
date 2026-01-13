#include "doctoreditview.h"
#include "ui_doctoreditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QSqlQueryModel>

DoctorEditView::DoctorEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::DoctorEditView)
{
    ui->setupUi(this);

    QSqlTableModel *deptModel = new QSqlTableModel(this, QSqlDatabase::database());
    deptModel->setTable("department");
    deptModel->select();
    ui->dbComboDept->setModel(deptModel);
    ui->dbComboDept->setModelColumn(deptModel->fieldIndex("DEPT_NAME"));

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().doctorTabModel;

    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditTitle, tabModel->fieldIndex("TITLE"));
    dataMapper->addMapping(ui->dbEditSpecialty, tabModel->fieldIndex("SPECIALTY"));
    dataMapper->addMapping(ui->dbComboDept, tabModel->fieldIndex("DEPT_ID"), "currentText");
    dataMapper->addMapping(ui->dbEditMobile, tabModel->fieldIndex("MOBILEPHONE"));
    dataMapper->addMapping(ui->dbEditSchedule, tabModel->fieldIndex("WORK_SCHEDULE"));
    dataMapper->addMapping(ui->dbCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(index);
}

DoctorEditView::~DoctorEditView()
{
    delete ui;
}

void DoctorEditView::on_btSave_clicked()
{
    IDatabase::getInstance().submitDoctorEdit();
    emit goPreviousView();
}

void DoctorEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertDoctorEdit();
    emit goPreviousView();
}
