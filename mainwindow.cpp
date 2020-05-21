#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent, Serializer *serializer) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnStart_clicked()
{
    emit beginSerial();
    // emit to Serializer
}

void MainWindow::on_btnStop_clicked()
{
    emit stopSerial();
    // emit to Serializer
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

void MainWindow::on_btnTerminate_clicked()
{
    emit terminateSerial();
}
