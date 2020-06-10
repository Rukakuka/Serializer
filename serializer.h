
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QMap>
#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QXmlStreamWriter>
#include <QTableView>
#include <QStandardItemModel>
#include <QDir>
#include <QUrl>
#include <QMatrix3x3>

#include "sensor.h"
#include "mainwindow.h"
#include "geometryestimation.h"

class Serializer : public QObject
{
    Q_OBJECT

public:
    Serializer(MainWindow *mainwindow);
    ~Serializer() {};


public slots:
    void Begin();
    void Stop();
    void SaveConfig(QString path);
    void LoadConfig(QString path);
    void changeConfigurationByUser(QList<Sensor*> sensors);

private:
    bool addDevice(QXmlStreamReader *reader, QList<Sensor*>* configuration);
    void addDeviceConfig(QXmlStreamReader *reader, QList<Sensor*>* configuration, int *deviceCount);
    void parseConfig(QXmlStreamReader *reader, QList<Sensor*>* configuration);
    QString whatConnectedPort(QList<QSerialPortInfo> portlist, Sensor *sensor);

    QList<Sensor*> currentConfiguration;
    QList<Sensor*> currentWorkingSensors;
    MainWindow *mainwindow;

    /* *** XML config file parameters *** */
    const QString defaultConfigurationPath = "E:/QtProjects/Serializer/configuration.xml";
    const QString rootName = "Configuration";
    const QString childrenName = "Device";
    const QString childrenAttributeName = "count";
    const QStringList chilrenFields = {"Port", "Identifier", "Name", "Baudrate", "Status"};    
    bool validatePortName(QString portname);

private slots:
    void setSensorData(qint16* data, quint64 timestamp);
    void setServiceData(Sensor::ServiceData data);
    void setSensorStatus(Sensor::SensorStatus status);

signals:
    void stopSerial();
    void beginSerial();
    void configurationChanged(QList<Sensor *> sensors);
    void sensorDataChanged(qint16* data, QString identifier);
    void serviceDataChanged(Sensor::ServiceData data, QString identifier);
    void sensorStatusChanged(Sensor::SensorStatus status, QString identifier);
};

#endif
