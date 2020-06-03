
#include "serializer.h"

Serializer::Serializer()
{
    QObject::connect(this, &Serializer::configurationChanged, this, &Serializer::ChangeConfiguration);
}

QList<QSerialPortInfo> Serializer::GetAvailablePorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    /*foreach(QSerialPortInfo port, ports)
    {
        qDebug() << port.manufacturer() << port.serialNumber() << port.productIdentifier() << port.vendorIdentifier();
    }*/

    return ports;
}

void Serializer::ChangeConfiguration(QList<Sensor*> newConfig)
{
    if (newConfig.isEmpty() || newConfig.count() == 0)
        return;
    foreach(Sensor* sens, newConfig)
    {
        configuration.clear();
        if(sens->Baudrate() > 0 && sens->Identifier().size() == 20)
        {
            this->configuration.append(sens);
        }
    }
}

QList<Sensor*>* Serializer::Begin(QList<QSerialPortInfo> portlist)
{    
    if (configuration.isEmpty())
    {
        LoadConfig(defaultConfigurationPath);
    }

    QList<Sensor*>* sensors = new QList<Sensor*>();

    for (int i = 0; i < configuration.count(); i++)
    {
        for (int j = 0; j < portlist.count(); j++)
        {
            if (portlist[j].serialNumber() == configuration.at(i)->Identifier())
            {

                Sensor *sensor = new Sensor(portlist[j].portName(),
                                            configuration.at(i)->Identifier(),
                                            configuration.at(i)->Baudrate(),
                                            configuration.at(i)->Name());
                QThread *thread = new QThread();

                sensor->moveToThread(thread);
                // automatically delete thread and task object when work is done:
                QObject::connect(sensor, SIGNAL(threadTerminating()), sensor, SLOT(deleteLater()));
                QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
                qDebug() << "Sensor " << sensor->Name() << " added";
                sensors->append(sensor);
                thread->start();
            }
        }
    }
    return sensors;
}

void Serializer::Stop()
{

}

void Serializer::SaveConfig(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement(rootName);

        for (int row = 0; row < this->configuration.count(); row++)
        {
            xmlWriter.writeStartElement(childrenName);
            xmlWriter.writeAttribute(childrenAttributeName, QString::number(row));
            xmlWriter.writeTextElement(chilrenFields[1], configuration[row]->Identifier());
            xmlWriter.writeTextElement(chilrenFields[2], configuration[row]->Name());
            xmlWriter.writeTextElement(chilrenFields[3], QString::number(configuration[row]->Baudrate()));
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndDocument();
        file.close();
    }
    else
    {
        qDebug() << "Cannot open file for write";
    }
}

QList<Sensor*> Serializer::LoadConfig(QString path)
{
    QFile file(path);
    QList<Sensor*> configuration;

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open configuration for read";
    }
    else
    {
        QXmlStreamReader reader(&file);
        ParseConfig(&reader, &configuration);
        emit configurationChanged(configuration);
    }
    return configuration;
}

void Serializer::ParseConfig(QXmlStreamReader* reader, QList<Sensor*>* configuration)
{
    qDebug() << "\nParsing configuration...";
    int deviceCount = -1;
    while (!reader->atEnd() && !reader->hasError())
    {
        QXmlStreamReader::TokenType token = reader->readNext();
        if (token == QXmlStreamReader::StartDocument)
        {
            continue;
        }
        if (token == QXmlStreamReader::StartElement)
        {
            if (reader->name() == rootName)
                continue;
            if (reader->name() == childrenName)
                AddDeviceConfig(reader, configuration, &deviceCount);
        }
    }
    qDebug() << "Parsing completed.";
}

void Serializer::AddDeviceConfig(QXmlStreamReader* reader, QList<Sensor*>* configuration, int *deviceCount)
{
    if (reader->tokenType() != QXmlStreamReader::StartElement && reader->name() == childrenName)
        return;
    QXmlStreamAttributes attributes = reader->attributes();
    if (attributes.hasAttribute(childrenAttributeName))
    {
        int num = attributes.value(childrenAttributeName).toInt();
        if (*deviceCount+1 == num)
        {
            *deviceCount = num;
            reader->readNext();
            if (!AddDevice(reader, configuration))
            {
                qDebug() << "Skip device at position" << num << " - error parsing fields.";
            }
        }
        else
        {
            qDebug() << "Skip device at position" << num << " - bad index.";
            do
            {
                reader->readNext();
            } while (reader->tokenType() != QXmlStreamReader::StartElement && reader->name() != childrenName && !reader->atEnd());
        }
    }
}

bool Serializer::AddDevice(QXmlStreamReader* reader, QList<Sensor*>* configuration)
{
    QString identifier = "";
    QString name = "";
    long baudrate = -1;

    while (reader->tokenType() != QXmlStreamReader::EndElement && reader->name() != childrenName)
    {
        if (reader->tokenType() == QXmlStreamReader::StartElement)
        {
            if(reader->name() == "Identifier")
            {
                QString str = reader->readElementText();
                bool duplicate = false;
                foreach(Sensor *sensor, *configuration)
                {
                    if (sensor->Identifier() == str)
                    {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate)
                    identifier = str;
                else
                    return false;
            }
            else if(reader->name() == "Name")
            {
                name = reader->readElementText();
                if (name.isNull() || name.isEmpty())
                    name = "NullName";
            }
            else if(reader->name() == "Baudrate")
            {
                long num = reader->readElementText().toLong();
                if (num > 0)
                    baudrate = num;
            }
            else if(reader->name() == "Port")
            {
                reader->readElementText();
            }
            else if(reader->name() == "Status")
            {
                reader->readElementText();
            }
        }
        reader->readNext();
    }
    if (!identifier.isEmpty() && baudrate > 0)
    {
        Sensor* s = new Sensor("", identifier, baudrate, name);
        configuration->append(s);
        return true;
    }
    return false;
}
