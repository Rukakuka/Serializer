#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sensor.h"
#include "serializer.h"

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class Mainwindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Serializer *serializer = nullptr);
    ~MainWindow();

private:
    Ui::Mainwindow *ui;
    Serializer *serializer;
    QList<QLineEdit*> *lineEditList;

public slots:
    void SetDataLabels(qint16 *databuf);
    void SetServiceData(qint64 *serviceData);
    void SetTableCurrentPorts(QList<Sensor*>* ports);

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
