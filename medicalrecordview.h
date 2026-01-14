#ifndef MEDICALRECORDVIEW_H
#define MEDICALRECORDVIEW_H

#include <QWidget>
#include <QTimer>



namespace Ui {
class MedicalRecordView;
}

class MedicalRecordView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicalRecordView(QWidget *parent = nullptr);
    ~MedicalRecordView();

signals:
    void goMedicalRecordEditView(int idx);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_btRefresh_clicked();
    void onDataLoaded();
    void delayedInit();
    void on_btAdd_clicked();

private:
    Ui::MedicalRecordView *ui;
    bool m_isInitialized;
    QTimer *m_initTimer;
    class QLabel *m_loadingLabel;  // 加载提示标签
};

#endif
