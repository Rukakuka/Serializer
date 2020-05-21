
#include "serializer.h"
#include "mainwindow.h"
#include "sensor.h"

#include <QApplication>
#include <QDebug>
#include <QVariant>
#include <QThread>
#include <QTimer>
#include <QPair>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Serializer serializer;
    MainWindow mainwindow(nullptr, &serializer);
    mainwindow.show();

    // TODO : move to serializer

    QList<QSerialPortInfo> portlist = serializer.GetAvailablePorts();

    QSerialPortInfo port;
    QList<Sensor*> sensors;

    foreach(port, portlist)
    {
        qDebug() << port.serialNumber();
        QMapIterator<QString, QString> iter(serializer.idList);
        while (iter.hasNext())
        {
            iter.next();
            if (port.serialNumber() == iter.key())
            {
                Sensor *sensor = new Sensor(&port, 2500000, iter.value()); //
                QThread *thread = new QThread();

                sensors.append(sensor);
                sensor->moveToThread(thread);

                // automatically delete thread and task object when work is done:
                QObject::connect(sensor, SIGNAL(threadTerminating()), sensor, SLOT(deleteLater()));
                QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

                QObject::connect(&mainwindow, SIGNAL(stopSerial()), sensor, SLOT(close()));
                QObject::connect(&mainwindow, SIGNAL(beginSerial()), sensor, SLOT(initialize()));
                QObject::connect(&mainwindow, SIGNAL(terminateSerial()), sensor, SLOT(terminateThread()));

                QObject::connect(sensor, SIGNAL(sendSensorData(qint16*)), &mainwindow, SLOT(SetDataLabels(qint16*)));
                QObject::connect(sensor, SIGNAL(sendSensorServiceData(qint64*)), &mainwindow, SLOT(SetServiceData(qint64*)));

                qDebug() << "Sensor added in thread : " << QThread::currentThreadId();
                thread->start();
            }
        }
    }
    qDebug() << "Setup done";
    // /TODO

    //qDebug() << sensors[0]->Name() << sensors[0]->Id();
    //qDebug() << QVariant::fromValue(sensors[0]->open()).toString();
    return app.exec();
}

