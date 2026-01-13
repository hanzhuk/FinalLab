#include "doctorview.h"
#include "ui_doctorview.h"
#include "idatabase.h"
#include "doctoreditview.h"

DoctorView::DoctorView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    IDatabase &iDatabase = IDatabase::getInstance();
    if (iDatabase.initDoctorModel()) {
        ui->tableView->setModel(iDatabase.doctorTabModel);
        ui->tableView->setSelectionModel(iDatabase.theDoctorSelection);
    }
}

DoctorView::~DoctorView()
{
    delete ui;
}

void DoctorView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewDoctor();
    emit goDoctorEditView(currow);
}

void DoctorView::on_btEdit_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theDoctorSelection->currentIndex();
    if (curIndex.isValid()) {
        emit goDoctorEditView(curIndex.row());
    }
}

void DoctorView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentDoctor();
}

void DoctorView::on_btSearch_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchDoctor(filter);
}
