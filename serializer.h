
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
#include <QVariant>
#include <QThread>
#include <QTimer>
#include <QPair>

#include "sensor.h"

class Serializer : public QObject
{
    Q_OBJECT

public:
    Serializer();
    ~Serializer() {};

    const QMap<QString, QString> idList = { {"9573535333235110F091", "LSM9DS1"},
                                            {"00000000000000000001", "xxxxxx"},
                                            {"00000000000000000002", "Dummy"}};

    QList<QSerialPortInfo> GetAvailablePorts();
    QList<Sensor*>* Begin(QList<QSerialPortInfo> portlist);
    Sensor* AddSensor(QSerialPortInfo port, QString name);

private:

};

#endif
