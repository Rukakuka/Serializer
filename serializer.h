
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
    QTableView* LoadConfig(QString path);

private:


    Sensor* AddSensor(QSerialPortInfo port, QString name, long baudrate);
    Sensor* AddSensor(QString identifier, QString name, long baudrate);

    bool AddElement(QXmlStreamReader *reader, QStandardItemModel *model, int &row);
    void AddDeviceConfig(QXmlStreamReader *reader, QStandardItemModel *model, int &row);
    void ParseConfig(QXmlStreamReader *reader, QStandardItemModel *model);

    const QString defaultConfigurationPath = "E:/QtProjects/Serializer/configuration.xml";

signals:
    void setNewConfig(QTableView*);
};

#endif
