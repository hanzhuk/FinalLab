#include "medicalrecordview.h"
#include "ui_medicalrecordview.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>

MedicalRecordView::MedicalRecordView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MedicalRecordView)
    , m_isInitialized(false)
    , m_initTimer(new QTimer(this))
{
    ui->setupUi(this);

    // 基础设置，不加载数据
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    // 设置加载提示
    ui->tableView->setModel(nullptr);

    // 连接定时器，延迟加载
    m_initTimer->setSingleShot(true);
    m_initTimer->setInterval(100);  // 延迟100ms
    connect(m_initTimer, &QTimer::timeout, this, &MedicalRecordView::delayedInit);
}

MedicalRecordView::~MedicalRecordView()
{
    delete ui;
}

void MedicalRecordView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // 延迟加载数据
    if (!m_isInitialized) {
        m_initTimer->start();
    }
}

void MedicalRecordView::delayedInit()
{
    // 显示加载动画
    QLabel *loadingLabel = new QLabel("正在加载数据，请稍候...", this);
    loadingLabel->setAlignment(Qt::AlignCenter);
    loadingLabel->setStyleSheet("font-size: 16px; color: #666;");
    ui->tableView->setVisible(false);

    // 使用 QTimer 异步执行加载
    QTimer::singleShot(50, [this]() {
        IDatabase &iDatabase = IDatabase::getInstance();
        if (iDatabase.initMedicalRecordModel()) {
            ui->tableView->setModel(iDatabase.recordTabModel);
            ui->tableView->setSelectionModel(iDatabase.theRecordSelection);
        }

        ui->tableView->setVisible(true);
        m_isInitialized = true;
    });
}

void MedicalRecordView::on_btRefresh_clicked()
{
    // 刷新时重新加载
    IDatabase::getInstance().updateRecordView();
}
