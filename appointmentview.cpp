#include "appointmentview.h"
#include "ui_appointmentview.h"
#include "idatabase.h"
#include <QMessageBox>

AppointmentView::AppointmentView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AppointmentView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    IDatabase &iDatabase = IDatabase::getInstance();
    if (iDatabase.initAppointmentModel()) {
        ui->tableView->setModel(iDatabase.appointmentTabModel);
        ui->tableView->setSelectionModel(iDatabase.theAppointmentSelection);
    }
}

AppointmentView::~AppointmentView()
{
    delete ui;
}

void AppointmentView::on_btRefresh_clicked()
{
    IDatabase::getInstance().updateAppointmentView();
}

void AppointmentView::on_btAddAppointment_clicked()
{
    QMessageBox::information(this, "提示", "请在患者模块中选择医生进行预约");
}

void AppointmentView::on_btConfirm_clicked()
{
    QMessageBox::information(this, "提示", "预约确认功能需关联具体患者");
}

void AppointmentView::on_btCancel_clicked()
{
    QMessageBox::information(this, "提示", "预约取消功能需关联具体患者");
}
