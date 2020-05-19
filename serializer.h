
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

class Serializer : public QObject
{
    Q_OBJECT

public:
    Serializer();
    ~Serializer() {};

    QMap<QString, QString> idList = { {"9573535333235110F091", "LSM9DS1"} };
    QList<QSerialPortInfo> GetAvailablePorts();

private:

};

#endif
