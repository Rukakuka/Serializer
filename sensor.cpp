#include "sensor.h"

Sensor::Sensor(QSerialPortInfo portinfo, long baudrate, QString name)
{
    qRegisterMetaType<Sensor::SensorStatus>();
    this->portname = portinfo.portName();
    this->identifier = portinfo.serialNumber();
    this->baudrate = baudrate;
    this->name = name;
    this->port = new QSerialPort(portinfo);
    setCurrentStatus(Sensor::READY);
}

Sensor::Sensor(QString identifier, long baudrate, QString name)
{
    qRegisterMetaType<Sensor::SensorStatus>();
    this->portname = "";
    this->identifier = identifier;
    this->baudrate = baudrate;
    this->name = name;
    this->port = new QSerialPort();
    setCurrentStatus(Sensor::OFFLINE);
}

Sensor::~Sensor()
{

}

void Sensor::setCurrentStatus(Sensor::SensorStatus st)
{
    currentStatus = st;
    emit statusChanged(st);
}

void Sensor::initialize()
{
    if (currentStatus == Sensor::READY)
    {
        this->isConnected = true;
        port->setBaudRate(baudrate);
        port->setPortName(portname);
        port->setParity(QSerialPort::NoParity);
        port->setDataBits(QSerialPort::Data8);
        port->setFlowControl(QSerialPort::NoFlowControl);

        receiveTimer = new QElapsedTimer();
        timeoutTimer = new QTimer();

        QObject::connect(port,SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::UniqueConnection);
        QObject::connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(readTimeout()), Qt::UniqueConnection);
        this->open();
    }
    else
    {        
        qDebug() << "Port" << this->name << "cannot be opened. Port status is" << currentStatus;
    }
}

void Sensor::open()
{
    if (port->open(QIODevice::ReadOnly))
    {
        port->clear(QSerialPort::AllDirections);
        receiveTimer->start();
        timeoutTimer->start(timeout);
        setCurrentStatus(Sensor::BUSY);
        qDebug() << "Sensor" << this->Name() << "initialized in thread" << QThread::currentThreadId();
    }
    else
    {
        setCurrentStatus(Sensor::PORT_OPEN_ERR);
        qDebug() << "Port" << this->name << "open error";
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
    if (currentStatus == Sensor::BUSY)
    {
        setCurrentStatus(Sensor::READY);
    }
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
                    qDebug() << "\n\nWrong packet size = " << pack.size() << " bytes, in port " << this->name;
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
            setCurrentStatus(Sensor::READ_ERROR);
            qDebug() << "RxBuffer too large after processing with size = " << rxbuf.size() << " bytes, in port " << this->name;
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
    setCurrentStatus(Sensor::TIMEOUT);
    qDebug() << "Serial read timeout. Port " << this->name << "closed.";
    this->close();
}

void Sensor::terminateThread()
{
    setCurrentStatus(Sensor::TERMINATED);
    qDebug() << "Thread terminating. Port " << this->name << "closed.";
    this->close();
    emit threadTerminating();
}

