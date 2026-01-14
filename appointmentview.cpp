#include "appointmentview.h"
#include "ui_appointmentview.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QLabel>

AppointmentView::AppointmentView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AppointmentView)
    , m_isInitialized(false)
    , m_initTimer(new QTimer(this))
    , m_loadingLabel(nullptr)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    m_loadingLabel = new QLabel("正在加载预约数据...", this);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setStyleSheet("font-size: 18px; color: #4A90E2; font-weight: bold;");
    m_loadingLabel->setVisible(true);

    ui->tableView->setVisible(false);

    m_initTimer->setSingleShot(true);
    m_initTimer->setInterval(100);
    connect(m_initTimer, &QTimer::timeout, this, &AppointmentView::delayedInit);  // ✅ 正确

    connect(&IDatabase::getInstance(), &IDatabase::appointmentLoaded,
            this, &AppointmentView::onDataLoaded);
}

AppointmentView::~AppointmentView()
{
    delete ui;
}

void AppointmentView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!m_isInitialized) {
        m_initTimer->start();
    }
}

void AppointmentView::delayedInit()  // ✅ 修复 3
{
    m_loadingLabel->setVisible(true);
    ui->tableView->setVisible(false);
    IDatabase::getInstance().loadAppointmentsAsync();
}

void AppointmentView::onDataLoaded()
{
    IDatabase &iDatabase = IDatabase::getInstance();
    ui->tableView->setModel(iDatabase.appointmentTabModel);
    ui->tableView->setSelectionModel(iDatabase.theAppointmentSelection);
    m_loadingLabel->setVisible(false);
    ui->tableView->setVisible(true);
    m_isInitialized = true;
}

void AppointmentView::on_btRefresh_clicked()
{
    if (m_isInitialized) {
        IDatabase::getInstance().updateAppointmentView();
    }
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
