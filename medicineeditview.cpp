#include "medicineeditview.h"
#include "ui_medicineeditview.h"
#include "idatabase.h"
#include <QSqlTableModel>

MedicineEditView::MedicineEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::MedicineEditView)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().medicineTabModel;

    dataMapper->setModel(tabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("MED_NAME"));
    dataMapper->addMapping(ui->dbEditSpec, tabModel->fieldIndex("MED_SPEC"));
    dataMapper->addMapping(ui->dbComboType, tabModel->fieldIndex("MED_TYPE"));
    dataMapper->addMapping(ui->dbSpinPrice, tabModel->fieldIndex("PRICE"));
    dataMapper->addMapping(ui->dbSpinStock, tabModel->fieldIndex("STOCK"));
    dataMapper->addMapping(ui->dbDateExpiry, tabModel->fieldIndex("EXPIRY_DATE"));
    dataMapper->addMapping(ui->dbCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(index);
}

MedicineEditView::~MedicineEditView()
{
    delete ui;
}

void MedicineEditView::on_btSave_clicked()
{
    IDatabase::getInstance().submitMedicineEdit();
    emit goPreviousView();
}

void MedicineEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertMedicineEdit();
    emit goPreviousView();
}
