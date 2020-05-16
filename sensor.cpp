#include "sensor.h"

Sensor::Sensor(QSerialPortInfo *portinfo, long baudrate, QString name)
{
    this->portinfo = portinfo;
    this->baudrate = baudrate;
    this->name = name;
    this->id = portinfo->serialNumber();
    this->port = new QSerialPort(*portinfo);
}

Sensor::SensorError Sensor::open()
{
    tmr = new QTimer();
    tmr->setInterval(1000);
    tmr->start();
    qDebug() << "timer setup done";
    QObject::connect(tmr, &QTimer::timeout, this, &Sensor::printFromAnotherThread);
    qDebug() << "timer connected";
    port->setBaudRate(baudrate);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setPortName(portinfo->portName());
    port->setFlowControl(QSerialPort::NoFlowControl);
    //port->setReadBufferSize(262144);

    if (port->open(QIODevice::ReadWrite))
    {
        return SensorError::NO_ERROR;
    }
    else
    {
        return SensorError::CANNOT_OPEN_PORT;
    }
}

void Sensor::printFromAnotherThread()
{
    qDebug() << "Hello from another thread" << QThread::currentThreadId();
}

void Sensor::finishWork()
{
    qDebug() << "Thread " << QThread::currentThreadId() << " is over.";
    emit workFinished();
}

