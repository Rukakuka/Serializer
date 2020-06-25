
#include "serializer.h"
#include "mainwindow.h"
#include "sensor.h"
#include "sensorgeometry.h"

#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QObject>
#include <QDebug>

void PrintNumbers()
{
    for (int i = 0; i <=1000; i++)
    {
        int th = i / 1000;
        int hu = (i - th*1000) / 100;
        int de = (i - th*1000 - hu*100) / 10;
        int si = i - de*10 - hu*100 - th*1000;

        int dsum = th + hu + de + si;
        qDebug() << i << dsum;

        if (dsum < 10 &&
                i % 3 == 0 &&
                i % 5 != 0 )
        {
            qDebug() << "The number:" << i;
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainwindow;
    Serializer serializer(&mainwindow);
    mainwindow.show();

    return app.exec();
}


