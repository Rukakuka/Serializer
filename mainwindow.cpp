#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
    qRegisterMetaType<QTableWidget*>();

    ports = new QList<Sensor*>();
    ui->setupUi(this);

    this->lineEditList = new QList<QLineEdit*>;
    this->ports = new QList<Sensor*>;

    lineEditList->append(ui->lineEditAx);
    lineEditList->append(ui->lineEditAy),
    lineEditList->append(ui->lineEditAz),
    lineEditList->append(ui->lineEditGx),
    lineEditList->append(ui->lineEditGy),
    lineEditList->append(ui->lineEditGz),
    lineEditList->append(ui->lineEditMx),
    lineEditList->append(ui->lineEditMy),
    lineEditList->append(ui->lineEditMz);

    QPair<QString, QString> pair;

    ui->tableCurrentConfig->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->tableCurrentConfig->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    QObject::connect(this, &MainWindow::sensorStatusChanged, this, &MainWindow::on_comboSelectPort_currentIndexChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetTableCurrentPorts(QList<Sensor*> ports)
{
    QStringList horizontalHeaderLabels = {"Port", "Identifier", "Name", "Baudrate", "Status"};

    ui->tableCurrentConfig->clear();
    ui->tableCurrentConfig->setRowCount(ports.size());
    ui->tableCurrentConfig->setColumnCount(horizontalHeaderLabels.size());
    ui->tableCurrentConfig->setShowGrid(true);
    ui->tableCurrentConfig->setHorizontalHeaderLabels(horizontalHeaderLabels);

    int row = 0;
    ui->comboSelectPort->clear();

    foreach (Sensor *sensor, ports)
    {
        QList<QString> columns;

        columns.append(sensor->Portname());
        columns.append(sensor->Identifier());
        columns.append(sensor->Name());
        columns.append(QString::number(sensor->Baudrate()));
        columns.append(QString(QVariant::fromValue(sensor->CurrentStatus()).toString()));

        ui->comboSelectPort->addItem(sensor->Name());
        ui->comboSelectPort->setItemData(ui->comboSelectPort->count()-1, sensor->Identifier(), Qt::UserRole);

        for (int col = 0; col < horizontalHeaderLabels.size(); col++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, columns.at(col));
            if (col == 0 || col == 4)
            {
               item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            }
            ui->tableCurrentConfig->setItem(row, col, item);
        }
        row++;
    }

    ui->tableCurrentConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    ui->tableCurrentConfig->resizeColumnsToContents();
    ui->tabWidget->setCurrentIndex(0);
    emit sensorStatusChanged(ui->comboSelectPort->currentIndex());
}

void MainWindow::SetSensorStatus(Sensor::SensorStatus status, QString identifier)
{
    int identifierColumn = whatColumnNumber("Identifier");
    int statusColumn = whatColumnNumber("Status");

    if (statusColumn != -1 && identifierColumn != -1)
    {
        for (int row = 0; row < ui->tableCurrentConfig->rowCount(); row++)
        {
            if (ui->tableCurrentConfig->item(row, identifierColumn)->text() == identifier)
            {
                ui->tableCurrentConfig->item(row, statusColumn)->setText(QVariant::fromValue(status).toString());
                emit sensorStatusChanged(ui->comboSelectPort->currentIndex());
            }
        }
    }
}

void MainWindow::SetSensorData(qint16 *databuf, QString identifier)
{
    QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(ui->comboSelectPort->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        for (int i = 0; i < lineEditList->size(); i++)
        {
            lineEditList->at(i)->setText(QString::number(databuf[i]));
        }
    }
}

void MainWindow::SetServiceData(Sensor::ServiceData data, QString identifier)
{
    QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(ui->comboSelectPort->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        ui->lineEditAverageLocalTime->setText(QString::number(1e12/((double)(data.LocalTimeElapsed))));
        ui->lineEditAverageRemoteTime->setText(QString::number(1e9/((double)(data.RemoteTimeElapsed))));
        ui->lineEditMissedPackets->setText(QString::number(data.DeclinedPackets));
    }
}

void MainWindow::on_btnStart_clicked()
{
    ui->btnLoadConfig->setEnabled(false);
    emit beginSerial();
}

void MainWindow::on_btnStop_clicked()
{
    ui->btnLoadConfig->setEnabled(true);
    emit stopSerial();
}

void MainWindow::on_btnLoadConfig_clicked()
{
    emit stopSerial();
    QString path = QFileDialog::getOpenFileName(this, "Open Configuration", "", "XML files (*.xml)");
    emit loadConfig(path);
}

void MainWindow::on_btnSaveConfig_clicked()
{
    QString path = QFileDialog::getSaveFileName(nullptr, "configuration", ".", "XML files (*.xml)" );
    emit saveConfig(path);
}

void MainWindow::on_btnAddDevice_clicked()
{
    ui->tableCurrentConfig->insertRow(ui->tableCurrentConfig->rowCount());
}

void MainWindow::on_btnRemoveDevice_clicked()
{
    QModelIndexList indexes =  ui->tableCurrentConfig->selectionModel()->selectedRows();
    int countRow = indexes.count();

    for( int i = countRow; i > 0; i--)
           ui->tableCurrentConfig->removeRow( indexes.at(i-1).row());
}

void MainWindow::on_comboSelectPort_currentIndexChanged(int index)
{
    int identifierColumn = whatColumnNumber("Identifier");
    int statusColumn = whatColumnNumber("Status");

    if (statusColumn != -1 && identifierColumn != -1)
    {
        QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(index, Qt::UserRole)).toString();
        if (!selectedIdentifier.isEmpty() && !selectedIdentifier.isNull())
        {
            for (int row = 0; row < ui->tableCurrentConfig->rowCount(); row++)
            {
                if (ui->tableCurrentConfig->item(row, identifierColumn)->text() == selectedIdentifier)
                {
                    ui->lineEditSensorStatus->setText(ui->tableCurrentConfig->item(row, statusColumn)->text());
                }
            }
        }
    }
}

int MainWindow::whatColumnNumber(QString name)
{
    for (int col= 0; col < ui->tableCurrentConfig->columnCount(); col++)
    {
        if (ui->tableCurrentConfig->horizontalHeaderItem(col)->text() == name)
        {
            return col;
        }
    }
    return -1;
}
