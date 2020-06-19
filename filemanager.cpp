#include "filemanager.h"

FileManager::FileManager(QObject *parent) : QObject(parent)
{

}

bool FileManager::Save(QString path, QList<Sensor*> *configuration)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("Configuration");
        for (int row = 0; row < configuration->count(); row++)
        {
            xmlWriter.writeStartElement("Device");
            xmlWriter.writeAttribute("count", QString::number(row));
            xmlWriter.writeTextElement("Identifier", configuration->at(row)->identifier());
            xmlWriter.writeTextElement("Name", configuration->at(row)->name());
            xmlWriter.writeTextElement("Baudrate", QString::number(configuration->at(row)->baudrate()));
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndDocument();
        file.close();
        return true;
    }
    return false;
}

bool FileManager::Load(QString path, QList<Sensor*>* configuration)
{
    QFile file(path);
    qDebug() << path;
    if (file.open(QIODevice::ReadOnly))
    {
        configuration->clear();
        QXmlStreamReader reader(&file);
        config_parse(&reader, configuration);
        file.close();
        return true;
    }
    return false;
}

bool FileManager::Save(QString path, QList<QVector3D*> *rawCalibrationData, QString identifier)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("RawMagnetometerData");
        xmlWriter.writeAttribute("identifier", identifier);
        for (int i = 0; i < rawCalibrationData->count(); i++)
        {
            xmlWriter.writeStartElement("Point");
            xmlWriter.writeAttribute("n", QString::number(i));
            xmlWriter.writeAttribute("mx", QString::number(rawCalibrationData->at(i)->x()));
            xmlWriter.writeAttribute("my", QString::number(rawCalibrationData->at(i)->y()));
            xmlWriter.writeAttribute("mz", QString::number(rawCalibrationData->at(i)->z()));
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndDocument();
        file.close();
        return true;
    }
    return false;
}

bool FileManager::Load(QString path, QList<QVector3D*> *rawCalibrationData)
{
    QFile file(path);
    qDebug() << path;
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader reader(&file);
        rawData_parse(&reader, rawCalibrationData);
        file.close();
        return true;
    }
    return false;
}

void FileManager::rawData_parse(QXmlStreamReader* reader, QList<QVector3D*>* rawData)
{
    qDebug() << "\nParsing configuration...";
    while (!reader->atEnd() && !reader->hasError())
    {
        QXmlStreamReader::TokenType token = reader->readNext();
        if (token == QXmlStreamReader::StartDocument)
        {
            continue;
        }
        if (token == QXmlStreamReader::StartElement)
        {
            if (reader->name() == "RawMagnetometerData")
                continue;
            if (reader->name() == "Point")
                rawData_parseData(reader, rawData);
        }
    }
    qDebug() << "Parsing completed.";
}

void FileManager::rawData_parseData(QXmlStreamReader* reader, QList<QVector3D*>* rawData)
{
    QVector3D *v = new QVector3D();
    QXmlStreamAttributes attributes = reader->attributes();
    if (attributes.hasAttribute("mz"))
    {
        v->setZ(attributes.value("mz").toInt());
    }
    if (attributes.hasAttribute("my"))
    {
        v->setY(attributes.value("my").toInt());
    }
    if (attributes.hasAttribute("mx"))
    {
        v->setX(attributes.value("mx").toInt());
    }
    rawData->append(v);
}


void FileManager::config_parse(QXmlStreamReader* reader, QList<Sensor*>* configuration)
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
            if (reader->name() == "Configuration")
                continue;
            if (reader->name() == "Device")
                config_parseDevice(reader, configuration, &deviceCount);
        }
    }
    qDebug() << "Parsing completed.";
}

void FileManager::config_parseDevice(QXmlStreamReader* reader, QList<Sensor*>* configuration, int *deviceCount)
{
    if (reader->tokenType() != QXmlStreamReader::StartElement && reader->name() == "Device")
        return;
    QXmlStreamAttributes attributes = reader->attributes();
    if (attributes.hasAttribute("count"))
    {
        int num = attributes.value("count").toInt();
        if (*deviceCount+1 == num)
        {
            *deviceCount = num;
            reader->readNext();
            if (!config_parseFields(reader, configuration))
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
            } while (reader->tokenType() != QXmlStreamReader::StartElement && reader->name() != "Device" && !reader->atEnd());
        }
    }
}

bool FileManager::config_parseFields(QXmlStreamReader* reader, QList<Sensor*>* configuration)
{
    QString identifier = "";
    QString name = "";
    long baudrate = -1;

    while (reader->tokenType() != QXmlStreamReader::EndElement && reader->name() != "Device")
    {
        if (reader->tokenType() == QXmlStreamReader::StartElement)
        {
            if(reader->name() == "Identifier")
            {
                QString str = reader->readElementText();
                bool duplicate = false;
                foreach(Sensor *sensor, *configuration)
                {
                    if (sensor->identifier() == str)
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
        QString port;
        QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
        for (int portNum = 0; portNum < portlist.count(); portNum++)
        {
            if (portlist[portNum].serialNumber() == identifier)
            {
                port = portlist[portNum].portName();
                break;
            }
        }
        Sensor* s = new Sensor(port, identifier, baudrate, name);
        configuration->append(s);
        return true;
    }
    return false;
}
