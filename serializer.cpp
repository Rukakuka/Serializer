
#include "serializer.h"

Serializer::Serializer()
{
    GetAvailablePorts();
}

QList<QSerialPortInfo>* Serializer::GetAvailablePorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>* list = new QList<QSerialPortInfo>();
    list = &ports;
    /*
    QSerialPortInfo port;

    foreach(port, ports)
    {
        qDebug() << port.manufacturer() << port.serialNumber() << port.productIdentifier() << port.vendorIdentifier();
    }
    */
    return list;
}

QList<Sensor*>* Serializer::Begin(QList<QSerialPortInfo>* portlist)
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
                Sensor* s = AddSensor(port, iter.value());
                list->append(s);
            }
        }
    }
    return list;
}

Sensor* Serializer::AddSensor(QSerialPortInfo port, QString name)
{
    static Sensor *sensor = new Sensor(&port, 2500000, name);
    QThread *thread = new QThread();

    sensor->moveToThread(thread);
    // automatically delete thread and task object when work is done:
    QObject::connect(sensor, SIGNAL(threadTerminating()), sensor, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    qDebug() << "Sensor added in thread : " << QThread::currentThreadId();

    thread->start();
    return sensor;
}
