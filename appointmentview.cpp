#include "appointmentview.h"
#include "ui_appointmentview.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QLabel>
#include <QTimer>  // 添加 QTimer 头文件

AppointmentView::AppointmentView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AppointmentView)
    , m_isInitialized(false)
    , m_initTimer(new QTimer(this))
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    // 初始化延迟加载定时器
    m_initTimer->setSingleShot(true);
    m_initTimer->setInterval(100);
    connect(m_initTimer, &QTimer::timeout, this, &AppointmentView::delayedInit);
}

AppointmentView::~AppointmentView()  // 添加析构函数实现
{
    delete ui;
}

void AppointmentView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // 延迟加载数据
    if (!m_isInitialized) {
        m_initTimer->start();
    }
}

void AppointmentView::delayedInit()
{
    // 使用 QTimer 异步加载（不阻塞UI线程）
    QTimer::singleShot(50, [this]() {
        IDatabase &iDatabase = IDatabase::getInstance();
        if (iDatabase.initAppointmentModel()) {
            ui->tableView->setModel(iDatabase.appointmentTabModel);
            ui->tableView->setSelectionModel(iDatabase.theAppointmentSelection);
            m_isInitialized = true;
        }
    });
}

// 添加缺失的槽函数实现
void AppointmentView::on_btRefresh_clicked()
{
    if (m_isInitialized) {
        IDatabase::getInstance().updateAppointmentView();
    }
}
