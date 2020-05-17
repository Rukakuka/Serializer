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

Q_DECLARE_METATYPE(QSerialPort::SerialPortError)
Q_DECLARE_METATYPE(QByteArray)

class Sensor : public QObject
{
    Q_OBJECT

public:

    enum SensorError
    {
        CANNOT_OPEN_PORT,
        WRONG_SENSOR_ID,
        WRONG_BAUDRATE,
        READING_ERROR,
        NO_ERROR
    };
    Q_ENUM(SensorError)
    explicit Sensor(QObject *parent = nullptr);

    Sensor(QSerialPortInfo *portinfo,
           long baudrate,
           QString name);
    ~Sensor();

    QSerialPortInfo *Portinfo() { return this->portinfo; }
    QString Name() { return this->name; }
    QString Id() { return this->id; }
    bool IsOnline() { return this->isOnline; }
    bool IsBusy() { return this->isBusy; }
    Sensor::SensorError open();
    Sensor::SensorError initialize();

private:
    long baudrate;
    bool isOnline;
    bool isBusy;
    QSerialPortInfo *portinfo;
    QString name;
    QString id;
    QSerialPort* port;
    QByteArray rxbuf;
    qint16 databuf[9];
    QElapsedTimer tmr;

public slots:
    void finishWork();
    void readyRead();
    void readError();
    void readTimeout();

private slots:
    void printFromAnotherThread();


signals:
    void workFinished();
    void sendSensorData(qint16 *databuf);
    void sendNanosElapsed(qint64 nsecs);

};

#endif // SENSOR_H
