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
    bool IsBusy() { return this->isBusy; }

private:
    const QString terminator = "\r\n";
    const long timeout = 5000;
    const int messageSize = 24;
    long baudrate;
    bool isBusy;
    QSerialPortInfo* portinfo;
    QString name;
    QSerialPort* port;
    QElapsedTimer* receiveTimer;
    QTimer* timeoutTimer;

    void open();

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
};

#endif // SENSOR_H
