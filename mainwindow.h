#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sensor.h"

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QApplication>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCoreApplication>
#include <QHeaderView>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Mainwindow; }
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QTableWidget*)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::Mainwindow *ui;
    QList<QLineEdit*> *lineEditList;
    QList<Sensor*> *ports;    
    int whatColumnNumber(QString name);

public slots:
    void SetSensorData(qint16 *databuf, QString identifier);
    void SetServiceData(Sensor::ServiceData data, QString identifier);
    void SetSensorStatus(Sensor::SensorStatus status, QString identifier);
    void SetTableCurrentPorts(QList<Sensor*> sensors);

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnLoadConfig_clicked();
    void on_btnSaveConfig_clicked();
    void on_btnAddDevice_clicked();
    void on_btnRemoveDevice_clicked();
    void on_comboSelectPort_currentIndexChanged(int index);

signals:
    void sensorStatusChanged(int comboBoxIndex);
    void stopSerial();
    void beginSerial();
    void saveConfig(QString);
    void loadConfig(QString);

};
#endif // SERIALIZER_H
