#include "statisticsview.h"
#include "ui_statisticsview.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

StatisticsView::StatisticsView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatisticsView)
{
    ui->setupUi(this);

    // 设置报表类型下拉框
    ui->comboReportType->addItem("患者统计", "patient");
    ui->comboReportType->addItem("药品预警", "medicine");
    ui->comboReportType->addItem("医生工作量", "doctor");
    ui->comboReportType->addItem("财务收入", "finance");

    // 设置日期范围默认值
    ui->dateStart->setDate(QDate::currentDate().addMonths(-1));
    ui->dateEnd->setDate(QDate::currentDate());
}

StatisticsView::~StatisticsView()
{
    delete ui;
}

void StatisticsView::displayReport(QJsonObject report)
{
    m_currentReport = report;

    ui->labelTitle->setText(report["title"].toString());
    ui->textBrowser->clear();

    QJsonDocument doc(report);
    ui->textBrowser->setPlainText(doc.toJson(QJsonDocument::Indented));

    setupChart(report);
    setupTable(report);

    ui->stackedWidget->setCurrentIndex(0);
}

void StatisticsView::showError(QString error)
{
    ui->labelTitle->setText("统计失败");
    ui->textBrowser->setPlainText(error);
    ui->stackedWidget->setCurrentIndex(0);
}

// 修改 on_btGenerate_clicked() 函数
void StatisticsView::on_btGenerate_clicked()
{
    QString reportType = ui->comboReportType->currentData().toString();
    QDate startDate = ui->dateStart->date();
    QDate endDate = ui->dateEnd->date();

    emit generateReport(reportType, startDate, endDate);  // ← 发射信号
}

void StatisticsView::on_btExport_clicked()
{
    if (m_currentReport.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先生成报表");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "导出报表",
                                                    QString("%1_%2.json").arg(m_currentReport["type"].toString())
                                                        .arg(QDate::currentDate().toString("yyyyMMdd")),
                                                    "JSON Files (*.json);;All Files (*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonDocument doc(m_currentReport);
            file.write(doc.toJson());
            file.close();
            QMessageBox::information(this, "成功", "报表已导出");
        }
    }
}

void StatisticsView::setupChart(const QJsonObject &data)
{
    // 这里可以集成 Qt Charts 或自定义绘图
    // 简化版：在 chartView 中显示柱状图示意
    Q_UNUSED(data)
}

void StatisticsView::setupTable(const QJsonObject &data)
{
    QJsonArray array = data["data"].toArray();
    if (array.isEmpty()) return;

    // 动态创建表格模型
    // 简化：在 tableWidget 中显示数据
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(array.size());

    QJsonObject firstObj = array[0].toObject();
    QStringList headers = firstObj.keys();
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        for (int j = 0; j < headers.size(); ++j) {
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(obj[headers[j]].toString()));
        }
    }
}
