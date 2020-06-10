
#include "serializer.h"
#include "mainwindow.h"
#include "sensor.h"
#include "geometryestimation.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainwindow;
    GeometryEstimation ge;
    Serializer serializer(&mainwindow);



    mainwindow.show();

    return app.exec();
}

