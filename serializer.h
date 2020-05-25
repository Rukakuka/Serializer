
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

#include "sensor.h"

class Serializer : public QObject
{
    Q_OBJECT

public:
    Serializer();
    ~Serializer() {};

    QTableView *configuration;

    QList<QSerialPortInfo> GetAvailablePorts();
    QList<Sensor*>* Begin(QList<QSerialPortInfo> portlist);

public slots:
    void SaveConfig(QTableWidget* table, QString path);
    void LoadConfig(QString path);

private:
    Sensor* AddSensor(QSerialPortInfo port, long baud, QString name);
    bool AddElement(QXmlStreamReader *reader, QStandardItemModel *model, int &row);
    void AddDeviceConfig(QXmlStreamReader *reader, QStandardItemModel *model, int &row);
    void ParseConfig(QXmlStreamReader *reader, QStandardItemModel *model);

    const QString defaultConfigurationName = "configuration.xml";

signals:
    void setNewConfig(QTableView*);
};

#endif
