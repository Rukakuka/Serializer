
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



    //qDebug() << sensors[0]->Name() << sensors[0]->Id();
    //qDebug() << QVariant::fromValue(sensors[0]->open()).toString();

    return app.exec();
}

