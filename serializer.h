
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

class Serializer : public QObject
{
    Q_OBJECT

public:
    Serializer();
    ~Serializer() {};

    QTableView *configuration;

    QList<QSerialPortInfo> GetAvailablePorts();


public slots:

    QList<Sensor*>* Begin(QList<QSerialPortInfo> portlist);
    void Stop();

    void SaveConfig(QTableWidget* table, QString path);
    QList<Sensor*>* LoadConfig(QString path);

private:
    Sensor* AddSensor(QSerialPortInfo port, QString name, long baudrate);
    Sensor* AddSensor(QString identifier, QString name, long baudrate);

    bool AddDevice(QXmlStreamReader *reader, QList<Sensor*>* configuration);
    void AddDeviceConfig(QXmlStreamReader *reader, QList<Sensor*>* configuration, int *deviceCount);
    void ParseConfig(QXmlStreamReader *reader, QList<Sensor*>* configuration);

    const QString defaultConfigurationPath = "E:/QtProjects/Serializer/configuration.xml";

    /* *** XML config file parameters *** */
    const QString rootName = "Configuration";
    const QString childrenName = "Device";
    const QString childrenAttributeName = "count";
    const QStringList chilrenFields = {"Port", "Identifier", "Name", "Baudrate", "Status"};

signals:
    void setNewConfig(QList<Sensor*>*);
};

#endif
