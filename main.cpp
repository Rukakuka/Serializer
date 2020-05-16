
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
                Sensor *s = new Sensor(&info, 115200, "LSM9DS1");
                QThread *thread = new QThread();
                sensors.append(s);                
                s->moveToThread(thread);
                // automatically delete thread and task object when work is done:
                QObject::connect(s, SIGNAL(workFinished()), s, SLOT(deleteLater()));
                QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
                QObject::connect(&mainwindow, SIGNAL(mysignal()), sensors[0], SLOT(finishWork()));

                qDebug() << "Added in thread : " << QThread::currentThreadId();

                thread->start();
                sensors[0]->open();
            }
        }
    }
    qDebug() << "setup done";

    //sensors[0]->finishWork();

    //qDebug() << sensors[0]->Name() << sensors[0]->Id();
    //qDebug() << QVariant::fromValue(sensors[0]->open()).toString();
    return app.exec();
}

