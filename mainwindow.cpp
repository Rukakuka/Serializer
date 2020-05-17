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
    ui->lineEditAx->setText(QString::number(databuf[0]));
    ui->lineEditAy->setText(QString::number(databuf[1]));
    ui->lineEditAz->setText(QString::number(databuf[2]));
    ui->lineEditGx->setText(QString::number(databuf[3]));
    ui->lineEditGy->setText(QString::number(databuf[4]));
    ui->lineEditGz->setText(QString::number(databuf[5]));
    ui->lineEditMx->setText(QString::number(databuf[6]));
    ui->lineEditMy->setText(QString::number(databuf[7]));
    ui->lineEditMz->setText(QString::number(databuf[8]));
}

void MainWindow::SetElapsedLabel(qint64 nsecs)
{
    ui->lineEditAvgTime->setText(QString::number(((double)nsecs)/1e9));
}

void MainWindow::on_btnTerminate_clicked()
{
    emit terminateSerial();
}
