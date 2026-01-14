#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class StatisticsView;
}

class StatisticsView : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    ~StatisticsView();

public slots:
    void displayReport(QJsonObject report);
    void showError(QString error);

private slots:
    void on_btGenerate_clicked();
    void on_btExport_clicked();

signals:
    void generateReport(QString type, QDate start, QDate end);

private:
    void setupChart(const QJsonObject &data);
    void setupTable(const QJsonObject &data);

    Ui::StatisticsView *ui;
    QJsonObject m_currentReport;
};

#endif // STATISTICSVIEW_H
