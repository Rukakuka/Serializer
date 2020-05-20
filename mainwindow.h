#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sensor.h"
#include "serializer.h"

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class Mainwindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::Mainwindow *ui;

public slots:
    void SetDataLabels(qint16 *databuf);
    void SetServiceData(qint64 *serviceData);

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();

    void on_btnTerminate_clicked();

signals:
    void stopSerial();
    void beginSerial();
    void terminateSerial();

};
#endif // SERIALIZER_H
