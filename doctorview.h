#ifndef DOCTORVIEW_H
#define DOCTORVIEW_H

#include <QWidget>

namespace Ui {
class DoctorView;
}

class DoctorView : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorView(QWidget *parent = nullptr);
    ~DoctorView();

signals:
    void goDoctorEditView(int idx);

private slots:
    void on_btAdd_clicked();
    void on_btEdit_clicked();
    void on_btDelete_clicked();
    void on_btSearch_clicked();

private:
    Ui::DoctorView *ui;
};

#endif
