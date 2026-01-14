#include "statisticsview.h"
#include "ui_statisticsview.h"

statisticsview::statisticsview(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::statisticsview)
{
    ui->setupUi(this);
}

statisticsview::~statisticsview()
{
    delete ui;
}
