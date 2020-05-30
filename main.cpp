
#include "serializer.h"
#include "mainwindow.h"
#include "sensor.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Serializer serializer;
    MainWindow mainwindow(nullptr, &serializer);

    QObject::connect(&serializer, SIGNAL(setNewConfig(QList<Sensor*>*)), &mainwindow, SLOT(SetTableCurrentPorts(QList<Sensor*>*)));

    mainwindow.show();

    QList<QSerialPortInfo> availablePorts = serializer.GetAvailablePorts();
    //QList<Sensor*>* ports = new QList<Sensor*>();

    QList<Sensor*>* ports = serializer.Begin(availablePorts);

    for (int i = 0; i < ports->size(); i++) // do sensor connections
    {
        if (true) //(ports->at(i)->CurrentStatus() != Sensor::OFFLINE)
        {
            QObject::connect(&mainwindow, SIGNAL(stopSerial()), ports->at(i), SLOT(close()));
            QObject::connect(&mainwindow, SIGNAL(beginSerial()), ports->at(i), SLOT(begin()));
            QObject::connect(&mainwindow, SIGNAL(terminateSerial()), ports->at(i), SLOT(terminateThread()));

            QObject::connect(ports->at(i), SIGNAL(sendSensorData(qint16*)), &mainwindow, SLOT(SetDataLabels(qint16*)));
            QObject::connect(ports->at(i), SIGNAL(sendSensorServiceData(Sensor::ServiceData*)), &mainwindow, SLOT(SetServiceData(Sensor::ServiceData*)));
        }
    }
    qDebug() << "Setup done in thread " << QThread::currentThreadId();

    //qDebug() << sensors[0]->Name() << sensors[0]->Id();
    //qDebug() << QVariant::fromValue(sensors[0]->open()).toString();

    return app.exec();
}

