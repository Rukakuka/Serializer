
#include "serializer.h"
#include "mainwindow.h"
#include "sensor.h"

#include <QApplication>
#include <QDebug>
#include <QVariant>
#include <QThread>
#include <QTimer>


const QList<QString> idList = {"9573535333235110F091"};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainwindow;
    mainwindow.show();

    Serializer serializer(&mainwindow);

    // TODO : move to serializer

    auto portlist = serializer.GetAvailablePorts();

    QSerialPortInfo info;
    QString id;
    QList<Sensor*> sensors;



    foreach(info,portlist)
    {
        foreach(id, idList)
        {
            if (info.serialNumber() == id)
            {
                Sensor *sensor = new Sensor(&info, 1286400, "LSM9DS1"); //
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
                QObject::connect(sensor, SIGNAL(sendNsecsElapsed(qint64)), &mainwindow, SLOT(SetElapsedLabel(qint64)));

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

