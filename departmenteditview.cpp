#include "departmenteditview.h"
#include "ui_departmenteditview.h"
#include "idatabase.h"
#include <QSqlTableModel>

DepartmentEditView::DepartmentEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::DepartmentEditView)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().departmentTabModel;

    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("DEPT_NAME"));
    dataMapper->addMapping(ui->dbEditDesc, tabModel->fieldIndex("DEPT_DESC"));
    dataMapper->addMapping(ui->dbEditLocation, tabModel->fieldIndex("LOCATION"));
    dataMapper->addMapping(ui->dbCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(index);
}

DepartmentEditView::~DepartmentEditView()
{
    delete ui;
}

void DepartmentEditView::on_btSave_clicked()
{
    IDatabase::getInstance().submitDepartmentEdit();
    emit goPreviousView();
}

void DepartmentEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertDepartmentEdit();
    emit goPreviousView();
}
