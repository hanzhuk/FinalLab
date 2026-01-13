#include "medicalrecordview.h"
#include "ui_medicalrecordview.h"
#include "idatabase.h"
#include <QMessageBox>

MedicalRecordView::MedicalRecordView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MedicalRecordView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    IDatabase &iDatabase = IDatabase::getInstance();
    if (iDatabase.initMedicalRecordModel()) {
        ui->tableView->setModel(iDatabase.recordTabModel);
        ui->tableView->setSelectionModel(iDatabase.theRecordSelection);
    }
}

MedicalRecordView::~MedicalRecordView()
{
    delete ui;
}

void MedicalRecordView::on_btRefresh_clicked()
{
    IDatabase::getInstance().updateRecordView();
}

void MedicalRecordView::on_btAddRecord_clicked()
{
    QMessageBox::information(this, "提示", "就诊记录应由医生在诊疗时创建，此处仅提供查看功能");
}
