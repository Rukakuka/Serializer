#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QByteArray>
#include <QMetaType>
#include <QElapsedTimer>

class Sensor : public QObject
{
    Q_OBJECT

public:
    enum SensorStatus
    {
        READ_ERROR,
        PORT_OPEN_ERR,
        TIMEOUT,
        READY,
        OFFLINE,
        BUSY,
        TERMINATED
    };    
    Q_ENUM(SensorStatus)

    explicit Sensor(QObject *parent = nullptr);

    Sensor(QSerialPortInfo portinfo,
           long baudrate,
           QString name);

    Sensor(QString identifier,
           long baudrate,
           QString name);

    ~Sensor();

    QString Identifier() { return this->identifier; }
    QString Portname() { return this->portname; }
    QString Name() { return this->name; }
    long Baudrate() { return this->baudrate; }
    SensorStatus CurrentStatus() { return currentStatus; }

private:

    const QString terminator = "\r\n";    
    const long timeout = 5000;
    const int messageSize = 24;

    long baudrate;
    bool isBusy;
    bool isConnected;
    QString identifier;
    QString portname;
    QString name;
    QSerialPort* port;
    QElapsedTimer* receiveTimer;
    QTimer* timeoutTimer;
    SensorStatus currentStatus;

    void open();
    void setCurrentStatus(SensorStatus st);

public slots:
    void initialize();
    void close();
    void terminateThread();

private slots:
    void readyRead();
    void readTimeout();
    void printFromAnotherThread();

signals:
    void threadTerminating();
    void sendSensorData(qint16 *databuf);
    void sendSensorServiceData(qint64* misc);
    void statusChanged(SensorStatus st);
};

Q_DECLARE_METATYPE(Sensor::SensorStatus)

#endif // SENSOR_H
