#include "medicalrecordview.h"
#include "ui_medicalrecordview.h"
#include "idatabase.h"
#include "medicalrecordeditview.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QtConcurrent>
#include <QTimer>
#include <QSqlDatabase>
#include <QDate>
#include <QDateTime>
#include <QMessageBox>
#include <QFuture>

MedicalRecordView::MedicalRecordView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MedicalRecordView)
    , m_isInitialized(false)
    , m_initTimer(new QTimer(this))
    , m_loadingLabel(nullptr)
{
    ui->setupUi(this);

    // 表格基础设置
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    // 创建加载提示标签
    m_loadingLabel = new QLabel("正在加载数据，请稍候...", this);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setStyleSheet("font-size: 18px; color: #4A90E2; font-weight: bold;");
    m_loadingLabel->setVisible(true);

    // 隐藏表格直到数据加载完成
    ui->tableView->setVisible(false);

    // 初始化定时器
    m_initTimer->setSingleShot(true);
    m_initTimer->setInterval(100);
    connect(m_initTimer, &QTimer::timeout, this, &MedicalRecordView::delayedInit);  // ✅ 修复 1

    // 连接数据库信号
    connect(&IDatabase::getInstance(), &IDatabase::medicalRecordLoaded,
            this, &MedicalRecordView::onDataLoaded);
}

MedicalRecordView::~MedicalRecordView()
{
    delete ui;
}

void MedicalRecordView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!m_isInitialized) {
        m_initTimer->start();
    }
}

void MedicalRecordView::delayedInit()  // ✅ 修复 2
{
    m_loadingLabel->setVisible(true);
    ui->tableView->setVisible(false);
    IDatabase::getInstance().loadMedicalRecordsAsync();
}

void MedicalRecordView::onDataLoaded()
{
    IDatabase &iDatabase = IDatabase::getInstance();
    ui->tableView->setModel(iDatabase.recordTabModel);
    ui->tableView->setSelectionModel(iDatabase.theRecordSelection);
    m_loadingLabel->setVisible(false);
    ui->tableView->setVisible(true);
    m_isInitialized = true;
}

void MedicalRecordView::on_btRefresh_clicked()
{
    if (m_isInitialized) {
        IDatabase::getInstance().updateRecordView();
    }
}

void MedicalRecordView::on_btAdd_clicked()
{
    emit goMedicalRecordEditView(-1);
}
