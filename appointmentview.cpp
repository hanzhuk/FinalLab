#include "appointmentview.h"
#include "ui_appointmentview.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

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
    QDialog dlg(this);
    dlg.setWindowTitle("新增预约");
    QFormLayout *form = new QFormLayout(&dlg);

    QComboBox *patientCombo = new QComboBox(&dlg);
    QComboBox *doctorCombo = new QComboBox(&dlg);
    QComboBox *deptCombo = new QComboBox(&dlg);
    QDateTimeEdit *timeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), &dlg);
    timeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    QComboBox *statusCombo = new QComboBox(&dlg);
    statusCombo->addItems(QStringList() << "待确认" << "已确认" << "已取消");

    form->addRow("患者姓名", patientCombo);
    form->addRow("医生姓名", doctorCombo);
    form->addRow("科室", deptCombo);
    form->addRow("预约时间", timeEdit);
    form->addRow("状态", statusCombo);

    QDialogButtonBox *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(btns);
    QObject::connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    // 先加载下拉数据
    QString connectionNameFill = QString("app_add_fill_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionNameFill);
        db.setDatabaseName("G:/Qt_file/community_medical.db");
        if (!db.open()) {
            QMessageBox::critical(this, "错误", "数据库打开失败：" + db.lastError().text());
            QSqlDatabase::removeDatabase(connectionNameFill);
            return;
        }
        {
            QSqlQuery q(db);
            q.exec("SELECT ID, NAME FROM patient ORDER BY NAME ASC");
            while (q.next()) patientCombo->addItem(q.value(1).toString(), q.value(0).toString());
        }
        {
            QSqlQuery q(db);
            q.exec("SELECT ID, NAME FROM doctor ORDER BY NAME ASC");
            while (q.next()) doctorCombo->addItem(q.value(1).toString(), q.value(0).toString());
        }
        {
            QSqlQuery q(db);
            q.exec("SELECT ID, DEPT_NAME FROM department ORDER BY DEPT_NAME ASC");
            while (q.next()) deptCombo->addItem(q.value(1).toString(), q.value(0).toString());
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionNameFill);

    if (dlg.exec() != QDialog::Accepted) return;

    // 再进行插入
    QString connectionNameIns = QString("app_add_ins_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionNameIns);
        db.setDatabaseName("G:/Qt_file/community_medical.db");
        if (!db.open()) {
            QMessageBox::critical(this, "错误", "数据库打开失败：" + db.lastError().text());
            QSqlDatabase::removeDatabase(connectionNameIns);
            return;
        }

        QString patientId = patientCombo->currentData().toString();
        QString doctorId  = doctorCombo->currentData().toString();
        QString deptId    = deptCombo->currentData().toString();

        if (patientId.isEmpty() || doctorId.isEmpty() || deptId.isEmpty()) {
            QMessageBox::critical(this, "错误", "患者/医生/科室名称未找到，请先在对应模块创建或选择正确名称");
            db.close();
            QSqlDatabase::removeDatabase(connectionNameIns);
            return;
        }

        QSqlQuery query(db);
        query.prepare("INSERT INTO appointment (ID, PATIENT_ID, DOCTOR_ID, DEPT_ID, APPOINT_TIME, STATUS, CREATEDTIMESTAMP) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(IDatabase::generateUUID());
        query.addBindValue(patientId);
        query.addBindValue(doctorId);
        query.addBindValue(deptId);
        query.addBindValue(timeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.addBindValue(statusCombo->currentText());
        query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

        if (!query.exec()) {
            QMessageBox::critical(this, "错误", "新增预约失败：" + query.lastError().text());
            db.close();
            QSqlDatabase::removeDatabase(connectionNameIns);
            return;
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionNameIns);

    IDatabase::getInstance().updateAppointmentView();
    QMessageBox::information(this, "成功", "预约创建成功");
}

void AppointmentView::on_btConfirm_clicked()
{
    QMessageBox::information(this, "提示", "预约确认功能需关联具体患者");
}

void AppointmentView::on_btCancel_clicked()
{
    QMessageBox::information(this, "提示", "预约取消功能需关联具体患者");
}
