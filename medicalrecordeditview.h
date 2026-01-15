#ifndef MEDICALRECORDEDITVIEW_H
#define MEDICALRECORDEDITVIEW_H

#include <QWidget>

namespace Ui {
class MedicalRecordEditView;
}

class MedicalRecordEditView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicalRecordEditView(QWidget *parent = nullptr, int index = 0);
    ~MedicalRecordEditView();

signals:
    void goPreviousView();

private slots:
    void on_btSave_clicked();
    void on_btCancel_clicked();

private:
    Ui::MedicalRecordEditView *ui;
};

#endif
