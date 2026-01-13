#ifndef DOCTOREDITVIEW_H
#define DOCTOREDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>

namespace Ui {
class DoctorEditView;
}

class DoctorEditView : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorEditView(QWidget *parent = nullptr, int index = 0);
    ~DoctorEditView();

signals:
    void goPreviousView();

private slots:
    void on_btSave_clicked();
    void on_btCancel_clicked();

private:
    Ui::DoctorEditView *ui;
    QDataWidgetMapper *dataMapper;
};

#endif
