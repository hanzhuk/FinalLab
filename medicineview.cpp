#include "medicineview.h"
#include "ui_medicineview.h"
#include "idatabase.h"
#include "medicineeditview.h"

MedicineView::MedicineView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MedicineView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    IDatabase &iDatabase = IDatabase::getInstance();
    if (iDatabase.initMedicineModel()) {
        ui->tableView->setModel(iDatabase.medicineTabModel);
        ui->tableView->setSelectionModel(iDatabase.theMedicineSelection);
    }
}

MedicineView::~MedicineView()
{
    delete ui;
}

void MedicineView::on_btAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewMedicine();
    emit goMedicineEditView(currow);
}

void MedicineView::on_btEdit_clicked()
{
    QModelIndex curIndex = IDatabase::getInstance().theMedicineSelection->currentIndex();
    if (curIndex.isValid()) {
        emit goMedicineEditView(curIndex.row());
    }
}

void MedicineView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentMedicine();
}

void MedicineView::on_btSearch_clicked()
{
    QString filter = QString("MED_NAME like '%%1%'").arg(ui->txtSearch->text());
    IDatabase::getInstance().searchMedicine(filter);
}
