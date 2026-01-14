// AppointmentView.h 同样添加延迟加载机制
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
    void delayedInit();

private:
    Ui::AppointmentView *ui;
    bool m_isInitialized;
    QTimer *m_initTimer;
};

#endif
