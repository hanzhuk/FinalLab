#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include "loginview.h"
#include "welcomeview.h"
#include "patientview.h"
#include "patienteditview.h"
#include "doctorview.h"
#include "doctoreditview.h"
#include "departmentview.h"
#include "departmenteditview.h"
#include "medicineview.h"
#include "medicineeditview.h"
#include "medicalrecordview.h"
#include "appointmentview.h"
#include "statisticsthread.h"
#include "statisticsview.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MasterView;
}
QT_END_NAMESPACE

class MasterView : public QWidget
{
    Q_OBJECT

public:
    MasterView(QWidget *parent = nullptr);
    ~MasterView();

public slots:
    void goLoginView();
    void goWelcomeView();
    void goPatientView();
    void goPatientEditView(int rowNo);
    void goDoctorView();
    void goDoctorEditView(int rowNo);
    void goDepartmentView();
    void goDepartmentEditView(int rowNo);
    void goMedicineView();
    void goMedicineEditView(int rowNo);
    void goMedicalRecordView();
    void goAppointmentView();
    void goPreviousView();
    void goStatisticsView();

private slots:
    void on_btBack_clicked();
    void on_stackedWidget_currentChanged(int arg1);
    void on_btLogout_clicked();
    void onStatisticsCompleted(QJsonObject result);
    void onStatisticsFailed(QString error);
    void on_btStatistics_clicked();

private:
    void pushWidgetToStackView(QWidget *widget);

    Ui::MasterView *ui;

    WelcomeView *welcomeView;
    LoginView *loginView;
    PatientView *patientView;
    PatientEditView *patientEditView;
    DoctorView *doctorView;
    DoctorEditView *doctorEditView;
    DepartmentView *departmentView;
    DepartmentEditView *departmentEditView;
    MedicineView *medicineView;
    MedicineEditView *medicineEditView;
    MedicalRecordView *medicalRecordView;
    AppointmentView *appointmentView;
    StatisticsThread *statisticsThread;
    StatisticsView *statisticsView;
};

#endif // MASTERVIEW_H
