#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Serializer *serializer) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
    qRegisterMetaType<QTableWidget*>();

    ui->setupUi(this);
    this->serializer = serializer;

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
    foreach (QString key, serializer->idList.keys())
    {
        ui->comboSelectPort->addItem(serializer->idList.value(key));
    }
    QObject::connect(this, &MainWindow::saveConfig, serializer, &Serializer::SaveConfig);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetTableCurrentPorts(QList<Sensor*>* ports)
{
    QStringList horizontalHeaderLabels = {"Port", "Unique ID", "Name", "Baudrate", "Status"};
    int columns = horizontalHeaderLabels.size();

    ui->tableCurrentConfig->setRowCount(ports->size());
    ui->tableCurrentConfig->setColumnCount(columns);
    ui->tableCurrentConfig->setShowGrid(true);
    ui->tableCurrentConfig->setHorizontalHeaderLabels(horizontalHeaderLabels);

    QListIterator<Sensor*> iter(*ports);

    int row = 0;
    while (iter.hasNext())
    {
        Sensor* sensor = iter.next();
        QList<QString> list;

        list.append(sensor->Portinfo().portName());
        list.append(sensor->Portinfo().serialNumber());
        list.append(sensor->Name());
        list.append(QString::number(sensor->Baudrate()));
        list.append("StatusName"); // TODO : Serizlizer::PortStatus::Ready / Busy / Offline

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


void MainWindow::SetDataLabels(qint16 *databuf)
{
    Sensor* sens = qobject_cast<Sensor*>(sender());
    if (sens->Name() == ui->comboSelectPort->currentText())
    {
        for (int i = 0; i < lineEditList->size(); i++)
        {
            lineEditList->at(i)->setText(QString::number(databuf[i]));
        }
    }
}

void MainWindow::SetServiceData(qint64 *serviceData)
{
    Sensor* sens = qobject_cast<Sensor*>(sender());
    if (sens->Name() == ui->comboSelectPort->currentText())
    {
        ui->lineEditAverageLocalTime->setText(QString::number(((double)serviceData[0])/1e9));
        ui->lineEditAverageRemoteTime->setText(QString::number(((double)(serviceData[1]))/1e6));
        ui->lineEditMissedPackets->setText(QString::number(serviceData[2]));
    }
}

void MainWindow::on_btnStart_clicked()
{
    emit beginSerial();
}

void MainWindow::on_btnStop_clicked()
{
    emit stopSerial();
}

void MainWindow::on_btnTerminate_clicked()
{
    emit terminateSerial();
}

void MainWindow::on_btnLoadConfig_clicked()
{
    emit loadConfig();
}

void MainWindow::on_btnSaveConfig_clicked()
{
    emit saveConfig(ui->tableCurrentConfig);
}
