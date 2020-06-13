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
        DISCONNECTED,
        READY,
        BUSY,
        TERMINATED,
        PARSE_ERR,
        PORT_OPEN_ERR,
        TIMEOUT_ERR,
    };    
    Q_ENUM(SensorStatus)

    struct ServiceData
    {
        double LocalTimeElapsed;
        double RemoteTimeElapsed;
        uint   DeclinedPackets;
    };

    explicit Sensor(QObject *parent = nullptr);

    Sensor(QString m_portname, QString m_identifier, long m_baudrate, QString m_name);

    QString identifier() { return m_identifier; }
    QString portname() { return m_portname; }
    QString name() { return m_name; }
    long baudrate() { return m_baudrate; }
    SensorStatus currentStatus() { return m_currentStatus; }

private:

    const QString terminator = "\r\n";    
    const long timeout = 100; // [ms]
    const int messageSize = 24;

    long m_baudrate;
    QString m_identifier;
    QString m_portname;
    QString m_name;
    QSerialPort* m_port;
    SensorStatus m_currentStatus;

    QElapsedTimer* receiveTimer;
    QTimer* timeoutTimer;


    /* readyRead slot vars */
    quint16 cnt = 0;
    QByteArray rxbuf;
    qint16 databuf [9];
    qint64 declinedPackets = 0;
    quint64 timestamp = 0;
    quint64 prev_timestamp = 0;

    void setCurrentStatus(SensorStatus st);

public slots:
    void begin();
    void stop();
    void terminateThread();

private slots:
    void readyRead();
    void readTimeout();

signals:
    void threadTerminating();
    void sensorDataChanged(qint16* data, quint64 timestamp);
    void serviceDataChanged(Sensor::ServiceData data);
    void statusChanged(Sensor::SensorStatus status);
};

Q_DECLARE_METATYPE(Sensor::SensorStatus)
Q_DECLARE_METATYPE(Sensor::ServiceData)
Q_DECLARE_METATYPE(qint16*)

#endif // SENSOR_H
