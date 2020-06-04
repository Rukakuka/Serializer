
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

#include "sensor.h"
#include "mainwindow.h"

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

private:

    MainWindow *mainwindow;

    bool AddDevice(QXmlStreamReader *reader, QList<Sensor*>* configuration);
    void AddDeviceConfig(QXmlStreamReader *reader, QList<Sensor*>* configuration, int *deviceCount);
    void ParseConfig(QXmlStreamReader *reader, QList<Sensor*>* configuration);

    QList<Sensor*> currentConfiguration;
    QList<Sensor*> currentWorkingSensors;

    /* *** XML config file parameters *** */
    const QString defaultConfigurationPath = "E:/QtProjects/Serializer/configuration.xml";
    const QString rootName = "Configuration";
    const QString childrenName = "Device";
    const QString childrenAttributeName = "count";
    const QStringList chilrenFields = {"Port", "Identifier", "Name", "Baudrate", "Status"};

private slots:
    void setSensorData(qint16* data);
    void setServiceData(Sensor::ServiceData data);
    void setSensorStatus(Sensor::SensorStatus status);

signals:    
    void configurationChanged(QList<Sensor *> newConfig);
    void stopSerial();
    void beginSerial();

    void sensorDataChanged(qint16* data, QString identifier);
    void serviceDataChanged(Sensor::ServiceData data, QString identifier);
    void sensorStatusChanged(Sensor::SensorStatus status, QString identifier);
};

#endif
