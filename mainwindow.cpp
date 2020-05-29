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

    QObject::connect(this, &MainWindow::saveConfig, serializer, &Serializer::SaveConfig);
    QObject::connect(this, &MainWindow::loadConfig, serializer, &Serializer::LoadConfig);

    ui->tableCurrentConfig->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->tableCurrentConfig->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetTableCurrentPorts(QList<Sensor*>* ports)
{
    QStringList horizontalHeaderLabels = {"Port", "Identifier", "Name", "Baudrate", "Status"};
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

void MainWindow::SetServiceData(Sensor::ServiceData *sd)
{
    Sensor* sens = qobject_cast<Sensor*>(sender());
    if (sens->Name() == ui->comboSelectPort->currentText())
    {
        ui->lineEditAverageLocalTime->setText(QString::number(((double)(sd->LocalTimeElapsed))/1e9));
        ui->lineEditAverageRemoteTime->setText(QString::number(((double)(sd->RemoteTimeElapsed))/1e6));
        ui->lineEditMissedPackets->setText(QString::number(sd->DeclinedPackets));
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
    QString path = QFileDialog::getOpenFileName(this, "Open Configuration", "", "XML files (*.xml)");
    emit loadConfig(path);
}

void MainWindow::on_btnSaveConfig_clicked()
{
    QString path = QFileDialog::getSaveFileName(nullptr, "configuration", ".", "XML files (*.xml)" );
    emit saveConfig(ui->tableCurrentConfig, path);
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
