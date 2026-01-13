#ifndef WELCOMEVIEW_H
#define WELCOMEVIEW_H



#include <QWidget>

namespace Ui {
class WelcomeView;
}

class WelcomeView : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeView(QWidget *parent = nullptr);
    ~WelcomeView();

private slots:
    void on_btDepartment_clicked();
    void on_btDoctor_clicked();
    void on_btPatient_clicked();
    void on_btMedicine_clicked();
    void on_btMedicalRecord_clicked();
    void on_btAppointment_clicked();

signals:
    void goDepartmentView();
    void goDoctorView();
    void goPatientView();
    void goMedicineView();
    void goMedicalRecordView();
    void goAppointmentView();

private:
    Ui::WelcomeView *ui;
};

#endif // WELCOMEVIEW_H
