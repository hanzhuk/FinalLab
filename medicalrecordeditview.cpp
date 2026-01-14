#include "medicalrecordeditview.h"
#include "ui_medicalrecordeditview.h"
#include "idatabase.h"
#include <QSqlTableModel>

MedicalRecordEditView::MedicalRecordEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::MedicalRecordEditView)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTabModel;

    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->setCurrentIndex(index);
}

MedicalRecordEditView::~MedicalRecordEditView()
{
    delete ui;
}

void MedicalRecordEditView::on_btSave_clicked()
{
    emit goPreviousView();
}

void MedicalRecordEditView::on_btCancel_clicked()
{
    emit goPreviousView();
}

