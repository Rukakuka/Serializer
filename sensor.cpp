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
   this->close();
   emit threadTerminating();
}


void Sensor::initialize()
{
    qDebug() << "Initialize in thread" << QThread::currentThreadId();

    port->setBaudRate(baudrate);
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setPortName(portinfo->portName());
    port->setFlowControl(QSerialPort::NoFlowControl);

    receiveTimer = new QElapsedTimer();
    timeoutTimer = new QTimer();

    QObject::connect(port,SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::UniqueConnection);
    QObject::connect(port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(readyRead()), Qt::UniqueConnection);
    QObject::connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(readTimeout()), Qt::UniqueConnection);

    this->open();
}

void Sensor::open()
{
    qDebug() << "Open in thread" << QThread::currentThreadId();

    if (port->open(QIODevice::ReadWrite))
    {
        port->clear(QSerialPort::AllDirections);
        isBusy = true;
        receiveTimer->start();
        timeoutTimer->start(timeout);
    }
}

void Sensor::close()
{
    port->clear(QSerialPort::AllDirections);
    port->close();
    isBusy = false;
}

void Sensor::printFromAnotherThread()
{
    qDebug() << "Hello from thread" << QThread::currentThreadId();
}

void Sensor::readyRead()
{
    static quint16 cnt = 0;
    static QByteArray rxbuf;
    static qint16 databuf [9];


    if (timeoutTimer->isActive())
    {
        timeoutTimer->start(timeout);
    }

    rxbuf.append(port->readAll());
    if (rxbuf.size() > 20) // 18 bytes of data + "\r\n" terminator
    {
        while (rxbuf.indexOf(terminator) != -1)
        {
            cnt++;
            int end = rxbuf.indexOf(terminator);

            if (end-18 < 0) // in case of damaged packet, size less than 18
            {
                rxbuf = rxbuf.mid(end+2);
                end = rxbuf.indexOf(terminator);
            }
            else
            {
                QByteArray pack = rxbuf.mid(end-18, 18);
                if (pack.size() < 18)
                {
                    qDebug() << "Wrong packet size = " << pack.size() << " bytes";
                    throw;
                }
                //QDebug deb = qDebug();
                for (int i = 0; i < 18; i += 2)
                {
                    databuf[i/2] = (qint16)(pack[i+1]*255 + pack[i]);
                    //deb << databuf[i];
                }
                rxbuf = rxbuf.mid(end+2);
                //delete[] databuf;
                emit sendSensorData(databuf);
            }
        }
        if (rxbuf.size() > 20)
        {
            qDebug() << "RxBuffer too large after processing with size = " << rxbuf.size() << " bytes";
            throw;
        }
        if (cnt >= 1000)
        {
            emit sendNsecsElapsed(receiveTimer->nsecsElapsed());
            receiveTimer->restart();
            cnt = 0;
        }
    }
}

void Sensor::readError()
{
    qDebug() << "Serial read error";
}

void Sensor::readTimeout()
{
    qDebug() << "Serial read timeout. Port closed.";
    timeoutTimer->stop();
    this->close();
}

void Sensor::terminateThread()
{
    isBusy = false;
    emit threadTerminating();
}

