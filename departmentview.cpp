#include "departmentview.h"
#include "ui_departmentview.h"
#include "idatabase.h"
#include "departmenteditview.h"

DepartmentView::DepartmentView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DepartmentView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    IDatabase &iDatabase = IDatabase::getInstance();
    if (iDatabase.initDepartmentModel()) {
        ui->tableView->setModel(iDatabase.departmentTabModel);
        ui->tableView->setSelectionModel(iDatabase.theDepartmentSelection);
    }
}

DepartmentView::~DepartmentView()
{
    delete ui;
}

void DepartmentView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewDepartment();
    emit goDepartmentEditView(currow);
}

void DepartmentView::on_btEdit_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theDepartmentSelection->currentIndex();
    if (curIndex.isValid()) {
        emit goDepartmentEditView(curIndex.row());
    }
}

void DepartmentView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentDepartment();
}

void DepartmentView::on_btSearch_clicked()
{
    QString filter = QString("DEPT_NAME like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchDepartment(filter);
}
