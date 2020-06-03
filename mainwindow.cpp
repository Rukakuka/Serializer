#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Serializer *serializer) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
    qRegisterMetaType<QTableWidget*>();

    ports = new QList<Sensor*>();
    ui->setupUi(this);
    this->serializer = serializer;

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

    QObject::connect(this, &MainWindow::saveConfig, serializer, &Serializer::SaveConfig);
    QObject::connect(this, &MainWindow::loadConfig, serializer, &Serializer::LoadConfig);

    ui->tableCurrentConfig->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->tableCurrentConfig->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetTableCurrentPorts(QList<Sensor*> ports)
{
    ui->comboSelectPort->clear();
    ui->tableCurrentConfig->clear();

    QStringList horizontalHeaderLabels = {"Port", "Identifier", "Name", "Baudrate", "Status"};
    int columns = horizontalHeaderLabels.size();

    ui->tableCurrentConfig->setRowCount(ports.size());
    ui->tableCurrentConfig->setColumnCount(columns);
    ui->tableCurrentConfig->setShowGrid(true);
    ui->tableCurrentConfig->setHorizontalHeaderLabels(horizontalHeaderLabels);

    QListIterator<Sensor*> iter(ports);

    int row = 0;
    while (iter.hasNext())
    {
        Sensor* sensor = iter.next();
        QList<QString> list;

        list.append(sensor->Portname());
        list.append(sensor->Identifier());
        list.append(sensor->Name());
        list.append(QString::number(sensor->Baudrate()));
        list.append(QString(QVariant::fromValue(sensor->CurrentStatus()).toString()));

        ui->comboSelectPort->addItem(sensor->Name());

        for (int col = 0; col < columns; col++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, list.at(col));
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
}

void MainWindow::SetNewSensorStatus(Sensor::SensorStatus status)
{
    Sensor* sensor = qobject_cast<Sensor*>(sender());

    static int identifierColumn = -1;
    if (identifierColumn == -1)
    {
        for (int col= 0; col < ui->tableCurrentConfig->columnCount(); col++)
        {
            if (ui->tableCurrentConfig->horizontalHeaderItem(col)->text() == "Identifier")
            {
                identifierColumn = col;
                break;
            }
        }
    }

    static int statusColumn = -1;
    if (statusColumn == -1)
    {
        for (int col= 0; col < ui->tableCurrentConfig->columnCount(); col++)
        {
            if (ui->tableCurrentConfig->horizontalHeaderItem(col)->text() == "Status")
            {
                statusColumn = col;
                break;
            }
        }
    }

    if (statusColumn != -1 && identifierColumn != -1)
    {
        for (int row = 0; row < ui->tableCurrentConfig->rowCount(); row++)
        {
            if (ui->tableCurrentConfig->item(row, identifierColumn)->text() == sensor->Identifier())
            {
                ui->tableCurrentConfig->item(row, statusColumn)->setText(QString(QVariant::fromValue(status).toString()));
            }
        }
    }
}

void MainWindow::SetDataLabels(qint16 *databuf)
{

    Sensor* sensor = qobject_cast<Sensor*>(sender());

    if (sensor == nullptr )
        return;

    if (sensor->CurrentStatus()== Sensor::TERMINATED)
        return;

    if (databuf == nullptr)
        return;

    if (sensor->Name() == ui->comboSelectPort->currentText())
    {
        for (int i = 0; i < lineEditList->size(); i++)
        {
            lineEditList->at(i)->setText(QString::number(databuf[i]));
        }
    }

}

void MainWindow::SetServiceData(Sensor::ServiceData *sd)
{
    Sensor* sensor = qobject_cast<Sensor*>(sender());
    if (sensor == nullptr )
        return;
    if (sensor->CurrentStatus()== Sensor::TERMINATED)
        return;
    if (sd == nullptr)
        return;
    if (sensor->Name() == ui->comboSelectPort->currentText())
    {
        ui->lineEditAverageLocalTime->setText(QString::number(1e12/((double)(sd->LocalTimeElapsed))));
        ui->lineEditAverageRemoteTime->setText(QString::number(1e9/((double)(sd->RemoteTimeElapsed))));
        ui->lineEditMissedPackets->setText(QString::number(sd->DeclinedPackets));
    }
}

void MainWindow::on_btnStart_clicked()
{
    ui->btnLoadConfig->setEnabled(false);
    ports = serializer->Begin(serializer->GetAvailablePorts());

    for (int i = 0; i < ports->size(); i++) // do sensor connections
    {
            QObject::connect(this, SIGNAL(stopSerial()), ports->at(i), SLOT(terminateThread()));
            QObject::connect(this, SIGNAL(beginSerial()), ports->at(i), SLOT(begin()));

            QObject::connect(ports->at(i), SIGNAL(sendSensorData(qint16*)), this, SLOT(SetDataLabels(qint16*)));
            QObject::connect(ports->at(i), SIGNAL(sendSensorServiceData(Sensor::ServiceData*)), this, SLOT(SetServiceData(Sensor::ServiceData*)));
            QObject::connect(ports->at(i), SIGNAL(statusChanged(Sensor::SensorStatus)), this, SLOT(SetNewSensorStatus(Sensor::SensorStatus)));
    }
    qDebug() << "Setup done in thread " << QThread::currentThreadId();
    emit beginSerial();
}

void MainWindow::on_btnStop_clicked()
{
    for (int i = 0; i < ports->size(); i++) // do sensor connections
    {
        QObject::disconnect(ports->at(i), SIGNAL(sendSensorData(qint16*)), this, SLOT(SetDataLabels(qint16*)));
        QObject::disconnect(ports->at(i), SIGNAL(sendSensorServiceData(Sensor::ServiceData*)), this, SLOT(SetServiceData(Sensor::ServiceData*)));
        QObject::disconnect(ports->at(i), SIGNAL(statusChanged(Sensor::SensorStatus)), this, SLOT(SetNewSensorStatus(Sensor::SensorStatus)));
    }

    ui->btnLoadConfig->setEnabled(true);
    emit stopSerial();

    for (int i = 0; i < ports->size(); i++)
    {
        QObject::disconnect(this, SIGNAL(stopSerial()), ports->at(i), SLOT(terminateThread()));
        QObject::disconnect(this, SIGNAL(beginSerial()), ports->at(i), SLOT(begin()));
    }
    delete ports;
    ports = nullptr;
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
