#include "sensor.h"

Sensor::Sensor(QSerialPortInfo *portinfo, long baudrate, QString name)
{
    qRegisterMetaType<QSerialPort::SerialPortError>();
    this->portinfo = portinfo;
    this->baudrate = baudrate;
    this->name = name;
    this->id = portinfo->serialNumber();
    this->port = new QSerialPort(*portinfo);
}


Sensor::SensorError Sensor::initialize()
{
    port->setBaudRate(baudrate);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setPortName(portinfo->portName());
    port->setFlowControl(QSerialPort::NoFlowControl);
    //port->setReadBufferSize(262144);

    connect(port,SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(readyRead()));

    return SensorError::NO_ERROR;

}

Sensor::SensorError Sensor::open()
{
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

void Sensor::readyRead()
{
    rxbuf.append(port->readAll());
    if (rxbuf.size() > 20)
    {
        int end = rxbuf.lastIndexOf("\r\n")+2;
        QByteArray pack = rxbuf.mid(end-20, 18);
        QDebug deb = qDebug();
        for (int i = 0; i < 18; i += 2)
        {
            databuf[i/2] = (qint16)(pack[i+1] << 8 | pack[i]);
            deb << databuf[i/2];
        }
        rxbuf = rxbuf.mid(end);
    }
}

void Sensor::readError()
{
    qDebug() << "Serial read error";
}

void Sensor::readTimeout()
{
    qDebug() << "Serial read timeout";
}

void Sensor::finishWork()
{
    qDebug() << "Thread " << QThread::currentThreadId() << " is over.";
    emit workFinished();
}

