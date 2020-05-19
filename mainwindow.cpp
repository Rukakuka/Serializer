#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
    ui->setupUi(this);
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
    static QList<QLineEdit*> list;
    if (sens->Name() == "LSM9DS1")
    {
        list = {ui->lineEditAx,
                ui->lineEditAy,
                ui->lineEditAz,
                ui->lineEditGx,
                ui->lineEditGy,
                ui->lineEditGz,
                ui->lineEditMx,
                ui->lineEditMy,
                ui->lineEditMz};
    }

    for (int i = 0; i < 9; i++)
    {
        list[i]->setText(QString::number(databuf[i]));
    }
}

void MainWindow::SetElapsedLabel(qint64 nsecs)
{
    ui->lineEditAvgTime->setText(QString::number(((double)nsecs)/1e9));
}

void MainWindow::SetMissedPacketsLabel(qint64 packets)
{
    ui->lineEditMissedPackets->setText(QString::number(packets));
}

void MainWindow::on_btnTerminate_clicked()
{
    emit terminateSerial();
}
