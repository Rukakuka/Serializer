
#include "serializer.h"

Serializer::Serializer()
{

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

QList<Sensor*>* Serializer::Begin(QList<QSerialPortInfo> portlist)
{    
    QDir dir("");
    dir.cdUp();
    QTableView* configuration = LoadConfig(dir.path() + defaultConfigurationName);

    QList<Sensor*>* list = new QList<Sensor*>();

    for (int row = 0; row < configuration->model()->rowCount(); row++)
    {
        bool sensorIsOnline = false;
        int i = 0;
        for (; i < portlist.count(); i++)
        {
            if (portlist[i].serialNumber() == configuration->model()->index(row, 0).data().toString())
            {
                sensorIsOnline = true;
                break;
            }
        }
        Sensor* sensor;
        if (sensorIsOnline)
        {
            sensor = this->AddSensor(portlist[i],
                                     configuration->model()->index(row, 1).data().toString(),
                                     configuration->model()->index(row, 2).data().toLongLong());
        }
        else
        {
            sensor = this->AddSensor(configuration->model()->index(row, 0).data().toString(),
                                     configuration->model()->index(row, 1).data().toString(),
                                     configuration->model()->index(row, 2).data().toLongLong());
        }
        list->append(sensor);
    }
    return list;
}

Sensor* Serializer::AddSensor(QString identifier, QString name, long baudrate)
{
    Sensor *sensor = new Sensor(identifier, baudrate, name);
    qDebug() << "Sensor " << name << " added (offline)";
    return sensor;
}

Sensor* Serializer::AddSensor(QSerialPortInfo port, QString name, long baudrate)
{
    Sensor *sensor = new Sensor(port, baudrate, name);
    QThread *thread = new QThread();

    sensor->moveToThread(thread);
    // automatically delete thread and task object when work is done:
    QObject::connect(sensor, SIGNAL(threadTerminating()), sensor, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    qDebug() << "Sensor " << name << " added (connected)";

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

QTableView* Serializer::LoadConfig(QString path)
{
    QFile file(path);
    QTableView *configuration = new QTableView();
    QStandardItemModel *model = new QStandardItemModel();
    model -> setColumnCount(3);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open configuration for read";
    }
    else
    {
        QXmlStreamReader reader(&file);
        ParseConfig(&reader, model);
        model->setHorizontalHeaderLabels({ "Identifier", "Name", "Baudrate" });

        configuration->setModel(model);
        //configuration->show();
    }
    emit setNewConfig(configuration);
    return configuration;
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
            model->setItem(row-1, 0, new QStandardItem(s));
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
        model->setItem(row-1, 1, new QStandardItem(s));
    }
    else if(reader->name() == "Baudrate")
    {
        long baud = reader->readElementText().toLong();
        if (baud != 0)
        {
            model->setItem(row-1, 2, new QStandardItem(QString::number(baud)));
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
