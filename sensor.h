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
    bool IsConnected() { return this->isConnected; }
    bool IsBusy() { return this->isBusy; }
    long Baudrate() { return this->baudrate; }

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
