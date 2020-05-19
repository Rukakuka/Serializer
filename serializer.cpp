
#include "serializer.h"

Serializer::Serializer()
{
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


