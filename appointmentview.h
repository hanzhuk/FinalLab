#ifndef APPOINTMENTVIEW_H
#define APPOINTMENTVIEW_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class AppointmentView;
}

class AppointmentView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentView(QWidget *parent = nullptr);
    ~AppointmentView();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_btRefresh_clicked();
    void on_btAddAppointment_clicked();
    void on_btConfirm_clicked();
    void on_btCancel_clicked();
    void onDataLoaded();
    void delayedInit();

private:
    Ui::AppointmentView *ui;
    bool m_isInitialized;
    QTimer *m_initTimer;
    class QLabel *m_loadingLabel;
};

#endif
