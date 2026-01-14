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

protected:
    void showEvent(QShowEvent *event) override;  // 添加显示事件

private slots:
    void on_btRefresh_clicked();
    void delayedInit();  // 添加延迟初始化槽

private:
    Ui::MedicalRecordView *ui;
    bool m_isInitialized;  // 标记是否已初始化
    QTimer *m_initTimer;   // 延迟加载定时器
};

#endif
