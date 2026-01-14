#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>
#include <QJsonObject>
#include <QDate>

namespace Ui {
class StatisticsView;
}

class StatisticsView : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    ~StatisticsView();

    void displayReport(QJsonObject report);
    void showError(QString error);

signals:
    void generateReport(QString type, QDate start, QDate end);

private slots:
    void on_btGenerate_clicked();
    void on_btExport_clicked();

private:
    Ui::StatisticsView *ui;
    QJsonObject m_currentReport;

    void setupChart(const QJsonObject &data);
    void setupTable(const QJsonObject &data);
};

#endif // STATISTICSVIEW_H
