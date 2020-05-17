#include "sensor.h"

Sensor::Sensor(QSerialPortInfo *portinfo, long baudrate, QString name)
{
    qRegisterMetaType<QSerialPort::SerialPortError>();
    qRegisterMetaType<QByteArray>();
    this->portinfo = portinfo;
    this->baudrate = baudrate;
    this->name = name;
    this->id = portinfo->serialNumber();
    this->port = new QSerialPort(*portinfo);
}

Sensor::~Sensor()
{
   port->close();
}


Sensor::SensorError Sensor::initialize()
{
    port->setBaudRate(baudrate);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setPortName(portinfo->portName());
    port->setFlowControl(QSerialPort::NoFlowControl);
    //port->setReadBufferSize(262144);
    tmr.start();
    QObject::connect(port,SIGNAL(readyRead()), this, SLOT(readyRead()));
    QObject::connect(port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(readyRead()));

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
    port->clear(QSerialPort::AllDirections);
}

void Sensor::printFromAnotherThread()
{
    qDebug() << "Hello from another thread" << QThread::currentThreadId();
}

void Sensor::readyRead()
{
    static quint16 cnt = 0;
    rxbuf.append(port->readAll());
    if (rxbuf.size() > 20)
    {
        while (rxbuf.indexOf(terminator) != -1)
        {
            cnt++;
            int end = rxbuf.indexOf(terminator);

            if (end-18 < 0) // in case of damaged not-full packet of bytes size=18
            {
                rxbuf = rxbuf.mid(end+2);
                end = rxbuf.indexOf(terminator);
                qDebug() << "removed packet2";
            }
            else
            {
                QByteArray pack = rxbuf.mid(end-18, 18);
                if (pack.size() < 18)
                {
                    throw;
                }
                //QDebug deb = qDebug();
                for (int i = 0; i < 18; i += 2)
                {
                    databuf[i/2] = (qint16)(pack[i+1]*255 + pack[i]);
                    //deb << databuf[i];
                }
                rxbuf = rxbuf.mid(end+2);
                emit sendSensorData(databuf);
            }
        }
        if (rxbuf.size() > 20)
        {
            throw;
        }
        if (cnt >= 1000)
        {
            qint64 ela = tmr.nsecsElapsed();
            emit sendNsecsElapsed(ela);
            tmr.restart();
            cnt = 1;
        }
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

