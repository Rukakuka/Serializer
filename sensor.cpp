#include "sensor.h"

Sensor::Sensor(QSerialPortInfo portinfo, long baudrate, QString name)
{
    qRegisterMetaType<QSerialPort::SerialPortError>();
    qRegisterMetaType<QByteArray>();

    this->portinfo = portinfo;
    this->baudrate = baudrate;
    this->name = name;
    this->port = new QSerialPort(portinfo);
    this->isBusy = false;
}

Sensor::~Sensor()
{

}

void Sensor::initialize()
{
    if (!isBusy)
    {
        port->setBaudRate(baudrate);
        port->setParity(QSerialPort::NoParity);
        port->setDataBits(QSerialPort::Data8);
        port->setPortName(portinfo.portName());
        port->setFlowControl(QSerialPort::NoFlowControl);
        receiveTimer = new QElapsedTimer();
        timeoutTimer = new QTimer();
        QObject::connect(port,SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::UniqueConnection);
        QObject::connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(readTimeout()), Qt::UniqueConnection);
        this->open();
    }
    else
    {
        qDebug() << "Port " << this->name << "is busy.";
    }
}

void Sensor::open()
{
    if (port->open(QIODevice::ReadOnly))
    {
        port->clear(QSerialPort::AllDirections);
        isBusy = true;
        receiveTimer->start();
        timeoutTimer->start(timeout);
    }
    else
    {
        qDebug() << "Err opening port " << this->name;
    }
}

void Sensor::close()
{
    if(port->isOpen())
    {
        port->clear(QSerialPort::AllDirections);
    }
    port->close();

    if (timeoutTimer != NULL)
    {
        timeoutTimer->stop();
        delete timeoutTimer;
        timeoutTimer = NULL;
        delete receiveTimer;
        receiveTimer = NULL;
    }

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
    static qint64 missedPackets = 0;
    static quint64 timestamp = 0;
    static quint64 prev_timestamp = 0;
    static int dataSize = messageSize-terminator.length();

    if (timeoutTimer->isActive())
    {
        timeoutTimer->start(timeout);
    }

    rxbuf.append(port->readAll());
    if (rxbuf.size() > messageSize) // 18 bytes of data + 4 bytes timestamp + "\r\n" terminator
    {
        while (rxbuf.indexOf(terminator) != -1)
        {
            cnt++;
            int end = rxbuf.indexOf(terminator);

            if (end-dataSize < 0) // in case of damaged packet, size less than 18+4
            {
                rxbuf = rxbuf.mid(end+2);
                end = rxbuf.indexOf(terminator);
                missedPackets++;
                //qDebug() << "Removed packet of data in " << this->name;
            }
            else
            {
                QByteArray pack = rxbuf.mid(end-dataSize, dataSize);
                if (pack.size() < dataSize)
                {
                    qDebug() << "Wrong packet size = " << pack.size() << " bytes, in port " << this->name;
                    throw;
                }
                //QDebug deb = qDebug();
                for (int i = 0; i < 18; i += 2)
                {
                    databuf[i/2] = (qint16)(pack[i+1]*255 + pack[i]);
                    //deb << databuf[i];
                }

                timestamp = quint64((unsigned char)(pack[21]) << 24 |
                                      (unsigned char)(pack[20]) << 16 |
                                       (unsigned char)(pack[19]) << 8 |
                                        (unsigned char)(pack[18]));

                rxbuf = rxbuf.mid(end+2);
                if (cnt % 50 == 0)
                {
                    emit sendSensorData(databuf);
                }
            }
        }
        if (rxbuf.size() > messageSize)
        {
            qDebug() << "RxBuffer too large after processing with size = " << rxbuf.size() << " bytes, in port " << this->name;
            //throw;
        }
        if (cnt >= 1000)
        {
            static qint64 serviceData[3];

            serviceData[0] = receiveTimer->nsecsElapsed();
            serviceData[1] = timestamp - prev_timestamp;
            serviceData[2] = missedPackets;

            emit sendSensorServiceData(serviceData);

            receiveTimer->restart();
            prev_timestamp = timestamp;
            missedPackets = 0;
            cnt = 0;
        }
    }
}

void Sensor::readTimeout()
{
    qDebug() << "Serial read timeout. Port " << this->name << "closed.";
    this->close();
}

void Sensor::terminateThread()
{
    qDebug() << "Thread terminating. Port " << this->name << "closed.";
    this->close();
    emit threadTerminating();
}

