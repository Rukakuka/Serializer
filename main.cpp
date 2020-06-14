
#include "serializer.h"
#include "mainwindow.h"
#include "sensor.h"
#include "sensorgeometry.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainwindow;
    Serializer serializer(&mainwindow);

    mainwindow.show();

    return app.exec();
}

