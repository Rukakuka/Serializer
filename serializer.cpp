
#include "serializer.h"

Serializer::Serializer(MainWindow* mainwindow)
{
    this->ui = mainwindow;
    GetAvailablePorts();
}

QList<QSerialPortInfo> Serializer::GetAvailablePorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    /*
    QSerialPortInfo port;

    foreach(port, ports)
    {
        qDebug() << port.manufacturer() << port.serialNumber() << port.productIdentifier() << port.vendorIdentifier();
    }
    */
    return ports;
}


