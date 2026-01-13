#ifndef MEDICINEVIEW_H
#define MEDICINEVIEW_H

#include <QWidget>

namespace Ui {
class MedicineView;
}

class MedicineView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineView(QWidget *parent = nullptr);
    ~MedicineView();

signals:
    void goMedicineEditView(int idx);

private slots:
    void on_btAdd_clicked();
    void on_btEdit_clicked();
    void on_btDelete_clicked();
    void on_btSearch_clicked();

private:
    Ui::MedicineView *ui;
};

#endif
