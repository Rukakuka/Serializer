
#include "serializer.h"

Serializer::Serializer()
{
    GetAvailablePorts();
}

QList<QSerialPortInfo> Serializer::GetAvailablePorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    /*
    QSerialPortInfo port;

    foreach(port, ports)
    {
        qDebug() << port.manufacturer() << port.serialNumber() << port.productIdentifier() << port.vendorIdentifier();
    }
    */
    return ports;
}

QList<Sensor*>* Serializer::Begin(QList<QSerialPortInfo> portlist)
{
    QSerialPortInfo port;
    QList<Sensor*>* list = new QList<Sensor*>();

    foreach(port, portlist)
    {
        qDebug() << port.serialNumber();

        QMapIterator<QString, QString> iter(this->idList);
        while (iter.hasNext())
        {
            iter.next();
            if (port.serialNumber() == iter.key())
            {
                Sensor* sensor = this->AddSensor(port, 2500000, iter.value());
                list->append(sensor);
            }
        }
    }
    return list;
}

void Serializer::SaveConfig(QTableWidget *table)
{
    int rows = table->rowCount();
    qDebug() << "Rows = " << rows;
}

Sensor* Serializer::AddSensor(QSerialPortInfo port, long baud, QString name)
{
    Sensor *sensor = new Sensor(port, baud, name);
    QThread *thread = new QThread();

    sensor->moveToThread(thread);
    // automatically delete thread and task object when work is done:
    QObject::connect(sensor, SIGNAL(threadTerminating()), sensor, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    qDebug() << "Sensor " << name << " added";

    thread->start();
    return sensor;
}

