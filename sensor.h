#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QThread>
#include <QDebug>


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
    ~Sensor() {};

    QSerialPortInfo *Portinfo() { return this->portinfo; }
    QString Name() { return this->name; }
    QString Id() { return this->id; }
    bool IsOnline() { return this->isOnline; }
    bool IsBusy() { return this->isBusy; }
    Sensor::SensorError open();    


private:
    long baudrate;
    bool isOnline;
    bool isBusy;
    QSerialPortInfo *portinfo;
    QString name;
    QString id;
    QSerialPort* port;
    QTimer *tmr;

public slots:
    void finishWork();
private slots:
    void printFromAnotherThread();
signals:
    void workFinished();
};

#endif // SENSOR_H
