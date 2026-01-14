#include "masterview.h"
#include "ui_masterview.h"
#include <QDebug>
#include "idatabase.h"
#include <QTimer>
#include <QMessageBox>

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
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);

    // 连接统计线程信号
    connect(statisticsThread, &StatisticsThread::statisticsCompleted,
            this, &MasterView::onStatisticsCompleted);
    connect(statisticsThread, &StatisticsThread::statisticsFailed,
            this, &MasterView::onStatisticsFailed);

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

void MasterView::on_btStatistics_clicked()
{
    goStatisticsView();
}
