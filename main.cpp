
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

    QList<QSerialPortInfo> availablePorts = serializer.GetAvailablePorts();
    //QList<Sensor*>* ports = new QList<Sensor*>();

    QList<Sensor*>* ports = serializer.Begin(availablePorts);

    for (int i = 0; i < ports->size(); i++)
    {
        QObject::connect(&mainwindow, SIGNAL(stopSerial()), ports->at(i), SLOT(close()));
        QObject::connect(&mainwindow, SIGNAL(beginSerial()), ports->at(i), SLOT(initialize()));
        QObject::connect(&mainwindow, SIGNAL(terminateSerial()), ports->at(i), SLOT(terminateThread()));

        QObject::connect(ports->at(i), SIGNAL(sendSensorData(qint16*)), &mainwindow, SLOT(SetDataLabels(qint16*)));
        QObject::connect(ports->at(i), SIGNAL(sendSensorServiceData(qint64*)), &mainwindow, SLOT(SetServiceData(qint64*)));
    }
    qDebug() << "Setup done";
    // /TODO

    //qDebug() << sensors[0]->Name() << sensors[0]->Id();
    //qDebug() << QVariant::fromValue(sensors[0]->open()).toString();
    mainwindow.SetTableCurrentPorts(ports);
    return app.exec();
}

