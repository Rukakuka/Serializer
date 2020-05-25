
#include "serializer.h"

Serializer::Serializer()
{

}

QList<QSerialPortInfo> Serializer::GetAvailablePorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo port, ports)
    {
        qDebug() << port.manufacturer() << port.serialNumber() << port.productIdentifier() << port.vendorIdentifier();
    }
    return ports;
}

QList<Sensor*>* Serializer::Begin(QList<QSerialPortInfo> portlist)
{
    LoadConfig(defaultConfigurationName);
    qDebug() << QFileInfo(".").absoluteDir();
    QSerialPortInfo port;
    QList<Sensor*>* list = new QList<Sensor*>();

    /*
    foreach(port, portlist)
    {
        QMapIterator<QString, QString> iter(this->idList);
        while (iter.hasNext())
        {
            iter.next();
            if (port.serialNumber() == iter.key())
            {
                Sensor* sensor = this->AddSensor(port, 2500000, iter.value());
                list->append(sensor);
            }
        }
    }*/
    return list;
}

Sensor* Serializer::AddSensor(QSerialPortInfo port, long baud, QString name)
{
    Sensor *sensor = new Sensor(port, baud, name);
    QThread *thread = new QThread();

    sensor->moveToThread(thread);
    // automatically delete thread and task object when work is done:
    QObject::connect(sensor, SIGNAL(threadTerminating()), sensor, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    qDebug() << "Sensor " << name << " added";

    thread->start();
    return sensor;
}

void Serializer::SaveConfig(QTableWidget *table, QString path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("Configuration");

        for (int row = 0; row < table->rowCount(); row++)
        {
            xmlWriter.writeStartElement("Device");
            xmlWriter.writeAttribute("count", QString::number(row));
            for (int col = 0; col < table->columnCount(); col++)
            {
                QString header = table->horizontalHeaderItem(col)->text();
                if (header != "Port" && header != "Status")
                {
                    xmlWriter.writeTextElement(table->horizontalHeaderItem(col)->text(), table->item(row,col)->text());
                }
            }
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

void Serializer::LoadConfig(QString path)
{
    QFile file(path);
    QTableView *configuration = new QTableView();
    QStandardItemModel *model = new QStandardItemModel();
    model -> setColumnCount(5);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open configuration for read";
        return;
    }

    QXmlStreamReader reader(&file);
    ParseConfig(&reader, model);
    configuration->setModel(model);

    configuration->show();


}

void Serializer::ParseConfig(QXmlStreamReader* reader, QStandardItemModel* model)
{
    qDebug() << "\nParsing configuration...";
    if (reader->readNextStartElement())
    {
        if (reader->name() == "Configuration")
        {
            int row = 0;
            while(reader->readNextStartElement())
            {
                AddDeviceConfig(reader, model, row);
            }
        }
    }
    qDebug() << "Parsing done.";
}

void Serializer::AddDeviceConfig(QXmlStreamReader* reader, QStandardItemModel* model, int &row)
{
    if(reader->name().contains("Device"))
    {
        QString count = reader->attributes().value("count").toString();
        model->insertRow(++row);
        while(reader->readNextStartElement())
        {
            if (!AddElement(reader, model, row))
            {
                model->removeRow(--row);
                qDebug() << "Skipped device configuration at position" << count;
            }
        }
    }
}

bool Serializer::AddElement(QXmlStreamReader* reader, QStandardItemModel* model, int &row)
{
    if(reader->name() == "Identifier")
    {
        QString s = reader->readElementText();
        if (model->findItems(s,Qt::MatchExactly).count() == 0)
        {
            model->setItem(row-1, 1, new QStandardItem(s));
        }
        else
        {
           reader->skipCurrentElement();
           return false;
        }
    }
    else if(reader->name() == "Name")
    {
        QString s = reader->readElementText();
        model->setItem(row-1, 2, new QStandardItem(s));
    }
    else if(reader->name() == "Baudrate")
    {
        long baud = reader->readElementText().toLong();
        if (baud != 0)
        {
            model->setItem(row-1, 3, new QStandardItem(QString::number(baud)));
        }
        else
        {
            return false;
        }
    }
    else
    {
        reader->skipCurrentElement();
    }
    return true;
}
