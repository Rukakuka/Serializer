#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
    qRegisterMetaType<QTableWidget*>();

    ui->setupUi(this);

    this->lineEditList = new QList<QLineEdit*>;

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

    ui->btnStop->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetConfigurationTable(QList<Sensor*> sensors)
{
    QStringList horizontalHeaderLabels = {"Port", "Identifier", "Name", "Baudrate", "Status"};

    ui->tableCurrentConfig->clear();
    ui->tableCurrentConfig->setRowCount(sensors.size());
    ui->tableCurrentConfig->setColumnCount(horizontalHeaderLabels.size());
    ui->tableCurrentConfig->setShowGrid(true);
    ui->tableCurrentConfig->setHorizontalHeaderLabels(horizontalHeaderLabels);

    int row = 0;
    ui->comboSelectPort->clear();

    foreach (Sensor *sensor, sensors)
    {
        if (sensor == nullptr)
            continue;

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
    on_comboSelectPort_currentIndexChanged(ui->comboSelectPort->currentIndex());
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
                on_comboSelectPort_currentIndexChanged(ui->comboSelectPort->currentIndex());
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
    ui->btnStop->setEnabled(true);
    ui->btnStart->setEnabled(false);
    emit beginSerial();
}

void MainWindow::on_btnStop_clicked()
{
    ui->btnLoadConfig->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->btnStart->setEnabled(true);
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
    if (!path.isNull() && !path.isEmpty())
    {
        QList<Sensor*> config = formatConfig(ui->tableCurrentConfig);
        emit configurationChangedByUser(config);
        emit saveConfig(path);
    }
}

QList<Sensor *> MainWindow::formatConfig(QTableWidget* table)
{
    QList<Sensor*> sensors;

    int identifierColumn = whatColumnNumber("Identifier");
    int baudrateColumn = whatColumnNumber("Baudrate");
    int nameColumn = whatColumnNumber("Name");

    if (identifierColumn != -1 && baudrateColumn !=-1 && nameColumn != -1)
    {
        for (int row = 0; row < ui->tableCurrentConfig->rowCount(); row++)
        {
            if (table->item(row, identifierColumn)->text().size() == 20 &&
                    table->item(row, baudrateColumn)->text().toLong() > 0 &&
                    table->item(row, nameColumn)->text().size() > 0)
            {

                Sensor *sensor = new Sensor("",
                                            table->item(row, identifierColumn)->text(),
                                            table->item(row, baudrateColumn)->text().toLong(),
                                            table->item(row, nameColumn)->text());
                sensors.append(sensor);
            }
            else
            {
                qDebug() << "Skipped sensor at position" << row+1 << ". Bad parameters";
            }
        }
    }
    return sensors;
}

void MainWindow::on_btnAddDevice_clicked()
{
    ui->tableCurrentConfig->insertRow(ui->tableCurrentConfig->rowCount());
}

void MainWindow::on_btnRemoveDevice_clicked()
{
    QModelIndexList indexes = ui->tableCurrentConfig->selectionModel()->selectedRows();
    int countRow = indexes.count();

    for( int i = countRow; i > 0; i--)
    {
        ui->tableCurrentConfig->removeRow(indexes.at(i-1).row());
    }
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
