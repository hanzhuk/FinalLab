#ifndef APPOINTMENTVIEW_H
#define APPOINTMENTVIEW_H

#include <QWidget>

namespace Ui {
class AppointmentView;
}

class AppointmentView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentView(QWidget *parent = nullptr);
    ~AppointmentView();

private slots:
    void on_btRefresh_clicked();
    void on_btAddAppointment_clicked();
    void on_btConfirm_clicked();
    void on_btCancel_clicked();

private:
    Ui::AppointmentView *ui;
};

#endif
