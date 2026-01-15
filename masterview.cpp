#include "masterview.h"
#include "ui_masterview.h"
#include <QDebug>
#include "idatabase.h"
#include <QTimer>
#include <QMessageBox>
#include "networkmanager.h"
#include "statisticsthread.h"
#include "statisticsview.h"
#include <QGroupBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include <QSqlQuery>

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
    , welcomeView(nullptr)
    , loginView(nullptr)
    , patientView(nullptr)
    , patientEditView(nullptr)
    , doctorView(nullptr)
    , doctorEditView(nullptr)
    , departmentView(nullptr)
    , departmentEditView(nullptr)
    , medicineView(nullptr)
    , medicineEditView(nullptr)
    , medicalRecordView(nullptr)
    , appointmentView(nullptr)
    , statisticsThread(new StatisticsThread(this))
    , statisticsView(nullptr)
    , networkManager(new NetworkManager(this))
{
    ui->setupUi(this);
    //this->setWindowFlag(Qt::FramelessWindowHint);

    // 连接统计线程信号
    connect(statisticsThread, &StatisticsThread::statisticsCompleted,
            this, &MasterView::onStatisticsCompleted);
    connect(statisticsThread, &StatisticsThread::statisticsFailed,
            this, &MasterView::onStatisticsFailed);

    // 连接网络管理器信号
    connect(networkManager, &NetworkManager::syncStarted,
            this, &MasterView::onSyncStarted);
    connect(networkManager, &NetworkManager::syncCompleted,
            this, &MasterView::onSyncCompleted);
    connect(networkManager, &NetworkManager::syncFailed,
            this, &MasterView::onSyncFailed);
    connect(networkManager, &NetworkManager::syncProgress,
            this, &MasterView::onSyncProgress);
    connect(networkManager, &NetworkManager::dataReceived,
            this, &MasterView::onDiagnosisData);
    connect(networkManager, &NetworkManager::medicineSynced,
            this, &MasterView::onMedicineSynced);

    goLoginView();

    IDatabase::getInstance();
}

MasterView::~MasterView()
{
    // 确保统计线程安全退出
    if (statisticsThread->isRunning()) {
        statisticsThread->quit();
        statisticsThread->wait();
    }

    // 网络管理器会自动清理，无需特别处理
    delete ui;
}

void MasterView::goLoginView()
{
    qDebug() << "goLoginView";
    loginView = new LoginView(this);
    pushWidgetToStackView(loginView);
    connect(loginView, SIGNAL(loginSuccess()), this, SLOT(goWelcomeView()));
}

void MasterView::goWelcomeView()
{
    qDebug() << "goWelcomeView";
    welcomeView = new WelcomeView(this);
    pushWidgetToStackView(welcomeView);

    connect(welcomeView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
    connect(welcomeView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
    connect(welcomeView, SIGNAL(goDepartmentView()), this, SLOT(goDepartmentView()));
    connect(welcomeView, SIGNAL(goMedicineView()), this, SLOT(goMedicineView()));
    connect(welcomeView, SIGNAL(goMedicalRecordView()), this, SLOT(goMedicalRecordView()));
    connect(welcomeView, SIGNAL(goAppointmentView()), this, SLOT(goAppointmentView()));
    connect(welcomeView, SIGNAL(goStatisticsView()), this, SLOT(goStatisticsView()));
    connect(welcomeView, SIGNAL(goNetworkSyncView()), this, SLOT(goNetworkSyncView()));
}

void MasterView::goPatientView()
{
    qDebug() << "goPatientView";
    patientView = new PatientView(this);
    pushWidgetToStackView(patientView);
    connect(patientView, SIGNAL(goPatientEditView(int)), this, SLOT(goPatientEditView(int)));
}

void MasterView::goPatientEditView(int rowNo)
{
    qDebug() << "goPatientEditView";
    patientEditView = new PatientEditView(this, rowNo);
    pushWidgetToStackView(patientEditView);
    connect(patientEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goDoctorView()
{
    qDebug() << "goDoctorView";
    doctorView = new DoctorView(this);
    pushWidgetToStackView(doctorView);
    connect(doctorView, SIGNAL(goDoctorEditView(int)), this, SLOT(goDoctorEditView(int)));
}

void MasterView::goDoctorEditView(int rowNo)
{
    qDebug() << "goDoctorEditView";
    doctorEditView = new DoctorEditView(this, rowNo);
    pushWidgetToStackView(doctorEditView);
    connect(doctorEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goDepartmentView()
{
    qDebug() << "goDepartmentView";
    departmentView = new DepartmentView(this);
    pushWidgetToStackView(departmentView);
    connect(departmentView, SIGNAL(goDepartmentEditView(int)), this, SLOT(goDepartmentEditView(int)));
}

void MasterView::goDepartmentEditView(int rowNo)
{
    qDebug() << "goDepartmentEditView";
    departmentEditView = new DepartmentEditView(this, rowNo);
    pushWidgetToStackView(departmentEditView);
    connect(departmentEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goMedicineView()
{
    qDebug() << "goMedicineView";
    medicineView = new MedicineView(this);
    pushWidgetToStackView(medicineView);
    connect(medicineView, SIGNAL(goMedicineEditView(int)), this, SLOT(goMedicineEditView(int)));
}

void MasterView::goMedicineEditView(int rowNo)
{
    qDebug() << "goMedicineEditView";
    medicineEditView = new MedicineEditView(this, rowNo);
    pushWidgetToStackView(medicineEditView);
    connect(medicineEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goMedicalRecordView()
{
    qDebug() << "goMedicalRecordView";
    medicalRecordView = new MedicalRecordView(this);
    pushWidgetToStackView(medicalRecordView);
    connect(medicalRecordView, SIGNAL(goMedicalRecordEditView(int)), this, SLOT(goMedicalRecordEditView(int)));
}

void MasterView::goMedicalRecordEditView(int rowNo)
{
    qDebug() << "goMedicalRecordEditView";
    medicalRecordEditView = new MedicalRecordEditView(this, rowNo);
    pushWidgetToStackView(medicalRecordEditView);
    connect(medicalRecordEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goAppointmentView()
{
    qDebug() << "goAppointmentView";
    appointmentView = new AppointmentView(this);
    pushWidgetToStackView(appointmentView);
}

void MasterView::goStatisticsView()
{
    qDebug() << "goStatisticsView";
    if (!statisticsView) {
        statisticsView = new StatisticsView(this);
        connect(statisticsView, &StatisticsView::generateReport,
                [this](const QString &type, const QDate &start, const QDate &end) {
                    statisticsThread->setReportType(type);
                    statisticsThread->setDateRange(start, end);
                    statisticsThread->start();
                });
    }
    pushWidgetToStackView(statisticsView);
}

void MasterView::goNetworkSyncView()
{
    qDebug() << "goNetworkSyncView";
    QWidget *syncWidget = new QWidget(this);
    syncWidget->setWindowTitle("网络同步");

    QVBoxLayout *mainLayout = new QVBoxLayout(syncWidget);

    QLabel *titleLabel = new QLabel("网络同步中心", syncWidget);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QGroupBox *statusGroup = new QGroupBox("服务器状态", syncWidget);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    QLabel *statusLabel = new QLabel("未连接", statusGroup);
    statusLayout->addWidget(statusLabel);
    mainLayout->addWidget(statusGroup);

    QGroupBox *syncGroup = new QGroupBox("同步操作", syncWidget);
    QVBoxLayout *syncLayout = new QVBoxLayout(syncGroup);

    QPushButton *btnMedicine = new QPushButton("🔄 同步药品数据库", syncGroup);
    btnMedicine->setStyleSheet("QPushButton { padding: 10px; text-align: left; }");
    syncLayout->addWidget(btnMedicine);

    QPushButton *btnDiagnosis = new QPushButton("📚 同步诊断参考", syncGroup);
    btnDiagnosis->setStyleSheet("QPushButton { padding: 10px; text-align: left; }");
    syncLayout->addWidget(btnDiagnosis);

    QPushButton *btnCheckUpdate = new QPushButton("⬇️ 检查更新", syncGroup);
    btnCheckUpdate->setStyleSheet("QPushButton { padding: 10px; text-align: left; }");
    syncLayout->addWidget(btnCheckUpdate);

    mainLayout->addWidget(syncGroup);

    QGroupBox *progressGroup = new QGroupBox("同步进度", syncWidget);
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);

    QProgressBar *progressBar = new QProgressBar(progressGroup);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressLayout->addWidget(progressBar);

    QLabel *progressLabel = new QLabel("准备就绪", progressGroup);
    progressLayout->addWidget(progressLabel);

    mainLayout->addWidget(progressGroup);

    connect(btnMedicine, &QPushButton::clicked, networkManager, &NetworkManager::syncMedicineDatabase);
    connect(btnDiagnosis, &QPushButton::clicked, networkManager, &NetworkManager::syncDiagnosisReference);
    connect(btnCheckUpdate, &QPushButton::clicked, networkManager, &NetworkManager::downloadUpdates);

    connect(networkManager, &NetworkManager::syncProgress, progressBar, &QProgressBar::setValue);
    connect(networkManager, &NetworkManager::syncStarted, progressLabel, [progressLabel](const QString &op) {
        progressLabel->setText("正在操作: " + op);
    });
    connect(networkManager, &NetworkManager::syncCompleted, progressLabel, [progressLabel](const QString &msg) {
        progressLabel->setText("✅ 完成: " + msg);
    });
    connect(networkManager, &NetworkManager::syncFailed, progressLabel, [progressLabel](const QString &err) {
        progressLabel->setText("❌ 错误: " + err);
    });

    QPushButton *btnBack = new QPushButton("← 返回主菜单", syncWidget);
    btnBack->setStyleSheet("QPushButton { padding: 10px; background-color: #f0f0f0; }");
    connect(btnBack, &QPushButton::clicked, this, &MasterView::goPreviousView);
    mainLayout->addWidget(btnBack);

    pushWidgetToStackView(syncWidget);
}

void MasterView::onStatisticsCompleted(QJsonObject result)
{
    if (statisticsView) {
        statisticsView->displayReport(result);
    }
}

void MasterView::onStatisticsFailed(QString error)
{
    if (statisticsView) {
        statisticsView->showError(error);
    }
}

void MasterView::onSyncStarted(QString operation)
{
    qDebug() << "同步开始:" << operation;
}

void MasterView::onSyncProgress(int percentage)
{
    qDebug() << "同步进度:" << percentage << "%";
}

void MasterView::onSyncCompleted(QString message)
{
    qDebug() << "同步完成:" << message;
}

void MasterView::onSyncFailed(QString error)
{
    qDebug() << "同步失败:" << error;
    QMessageBox::critical(this, "同步失败", error);
}

void MasterView::onDiagnosisData(QJsonObject data)
{
    QJsonArray arr = data.value("data").toArray();
    QDialog dlg(this);
    dlg.setWindowTitle("诊断参考（本地数据库）");
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QTableView *table = new QTableView(&dlg);
    QStandardItemModel *model = new QStandardItemModel(&dlg);
    model->setHorizontalHeaderLabels({"代码", "名称"});
    {
        QSqlQuery q;
        q.exec("SELECT CODE, NAME FROM diagnosis_reference ORDER BY CODE ASC");
        while (q.next()) {
            QList<QStandardItem*> items;
            items << new QStandardItem(q.value(0).toString());
            items << new QStandardItem(q.value(1).toString());
            model->appendRow(items);
        }
    }
    table->setModel(model);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    layout->addWidget(table);
    QPushButton *ok = new QPushButton("关闭", &dlg);
    connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(ok);
    dlg.exec();
}

void MasterView::onMedicineSynced(int count)
{
    QDialog dlg(this);
    dlg.setWindowTitle(QString("药品同步（本地数据库，%1 条）").arg(count));
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QTableView *table = new QTableView(&dlg);
    QStandardItemModel *model = new QStandardItemModel(&dlg);
    model->setHorizontalHeaderLabels({"ID", "名称", "规格", "类型", "价格", "库存", "有效期"});
    {
        QSqlQuery q;
        q.exec("SELECT ID, MED_NAME, MED_SPEC, MED_TYPE, PRICE, STOCK, EXPIRY_DATE FROM medicine ORDER BY ID ASC");
        while (q.next()) {
            QList<QStandardItem*> items;
            items << new QStandardItem(q.value(0).toString());
            items << new QStandardItem(q.value(1).toString());
            items << new QStandardItem(q.value(2).toString());
            items << new QStandardItem(q.value(3).toString());
            items << new QStandardItem(q.value(4).toString());
            items << new QStandardItem(q.value(5).toString());
            items << new QStandardItem(q.value(6).toString());
            model->appendRow(items);
        }
    }
    table->setModel(model);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    layout->addWidget(table);
    QPushButton *ok = new QPushButton("关闭", &dlg);
    connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(ok);
    dlg.exec();
}

void MasterView::on_btStatistics_clicked()
{
    goStatisticsView();
}

void MasterView::goPreviousView()
{
    int count = ui->stackedWidget->count();
    if (count > 1) {
        ui->stackedWidget->setCurrentIndex(count - 2);
        ui->labelTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

        QWidget *widget = ui->stackedWidget->widget(count - 1);
        ui->stackedWidget->removeWidget(widget);
        delete widget;
    }
}

void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int count = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(count - 1);
    ui->labelTitle->setText(widget->windowTitle());
}

void MasterView::on_btBack_clicked()
{
    goPreviousView();
}

void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    Q_UNUSED(arg1);

    int count = ui->stackedWidget->count();
    ui->btBack->setEnabled(count > 1);

    QString title = ui->stackedWidget->currentWidget()->windowTitle();
    if (title == "欢迎") {
        ui->btLogout->setEnabled(true);
        ui->btBack->setEnabled(false);
    } else {
        ui->btLogout->setEnabled(false);
    }
}

void MasterView::on_btLogout_clicked()
{
    goPreviousView();
}
