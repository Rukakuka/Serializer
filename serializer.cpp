
#include "serializer.h"

Serializer::Serializer(MainWindow *mainwindow)
{
    QObject::connect(this, &Serializer::configurationChanged, mainwindow, &MainWindow::setConfigurationTable);
    QObject::connect(this, &Serializer::sensorDataChanged, mainwindow, &MainWindow::setSensorData);
    QObject::connect(this, &Serializer::serviceDataChanged, mainwindow, &MainWindow::setServiceData);
    QObject::connect(this, &Serializer::sensorStatusChanged, mainwindow, &MainWindow::setSensorStatus);
    QObject::connect(this, &Serializer::sensorPoseChanged, mainwindow, &MainWindow::setSensorPose);

    QObject::connect(mainwindow, &MainWindow::saveConfig, this, &Serializer::SaveConfig);
    QObject::connect(mainwindow, &MainWindow::loadConfig, this, &Serializer::LoadConfig);
    QObject::connect(mainwindow, &MainWindow::beginSerial, this, &Serializer::Begin);
    QObject::connect(mainwindow, &MainWindow::stopSerial, this, &Serializer::Stop);
    QObject::connect(mainwindow, &MainWindow::configurationChangedByUser, this, &Serializer::changeConfigurationByUser);

    this->mainwindow = mainwindow;
}

void Serializer::Begin()
{    
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    if (currentConfiguration.isEmpty())
    {
        LoadConfig(defaultConfigurationPath);
    }
    currentWorkingSensors.clear();

    for (int devNum = 0; devNum < currentConfiguration.count(); devNum++)
    {
        QString portname = whatConnectedPort(portlist, currentConfiguration.at(devNum));

        if (validatePortName(portname))
        {
            Sensor *sensor = new Sensor(portname,
                                        currentConfiguration.at(devNum)->identifier(),
                                        currentConfiguration.at(devNum)->baudrate(),
                                        currentConfiguration.at(devNum)->name());
            SensorGeometry *sg = new SensorGeometry(sensor->identifier());
            QThread *thread = new QThread();
            sensor->moveToThread(thread);
            sg->moveToThread(thread);

            // obj live & death connections
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            QObject::connect(sensor, &Sensor::threadTerminating, sensor, &Sensor::deleteLater);            
            QObject::connect(this, &Serializer::stopSerial, sensor, &Sensor::terminateThread);
            QObject::connect(this, &Serializer::beginSerial, sensor, &Sensor::begin);

            // outcoming data connections
            QObject::connect(sensor, &Sensor::sensorDataChanged, this, &Serializer::setSensorData);
            QObject::connect(sensor, &Sensor::serviceDataChanged, this, &Serializer::setServiceData);
            QObject::connect(sensor, &Sensor::statusChanged, this, &Serializer::setSensorStatus);

            QObject::connect(sensor, &Sensor::sensorDataChanged, sg, &SensorGeometry::calculateNewPose);
            QObject::connect(sg, &SensorGeometry::poseChanged, this, &Serializer::setSensorPose);
            QObject::connect(sensor, &Sensor::threadTerminating, sg, &SensorGeometry::deleteLater);

            qDebug() << "Sensor " << sensor->name() << " added";
            currentWorkingSensors.append(sensor);
            delete currentConfiguration.at(devNum);
            currentConfiguration.replace(devNum, new Sensor(portname,
                                                            currentConfiguration.at(devNum)->identifier(),
                                                            currentConfiguration.at(devNum)->baudrate(),
                                                            currentConfiguration.at(devNum)->name()));
            emit  configurationChanged(currentConfiguration);
            thread->start();
        }
        else
        {
            delete currentConfiguration.at(devNum);
            currentConfiguration.replace(devNum, new Sensor("",
                                                            currentConfiguration.at(devNum)->identifier(),
                                                            currentConfiguration.at(devNum)->baudrate(),
                                                            currentConfiguration.at(devNum)->name()));
            emit  configurationChanged(currentConfiguration);
        }
    }
    emit beginSerial();
    qDebug() << "Setup done in thread " << QThread::currentThreadId();
}

QString Serializer::whatConnectedPort(QList<QSerialPortInfo> portlist, Sensor* sensor)
{
    for (int portNum = 0; portNum < portlist.count(); portNum++)
    {
        if (portlist[portNum].serialNumber() == sensor->identifier())
        {
            return portlist[portNum].portName();
        }
    }
    return "";
}

bool Serializer::validatePortName(QString portname)
{
    if (portname.contains("COM") && portname.mid(3).toInt() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Serializer::Stop()
{
    for (int i = 0; i < currentWorkingSensors.size(); i++) // do sensor connections
    {
        QObject::disconnect(currentWorkingSensors.at(i), &Sensor::sensorDataChanged, this, &Serializer::setSensorData);
        QObject::disconnect(currentWorkingSensors.at(i), &Sensor::serviceDataChanged, this, &Serializer::setServiceData);
    }

    emit stopSerial();

    for (int i = 0; i < currentWorkingSensors.size(); i++)
    {
        //QObject::disconnect(currentWorkingSensors.at(i), &Sensor::statusChanged, this, &Serializer::setSensorStatus);
        QObject::disconnect(this, &Serializer::stopSerial, currentWorkingSensors.at(i), &Sensor::terminateThread);
        QObject::disconnect(this, &Serializer::beginSerial, currentWorkingSensors.at(i), &Sensor::begin);
    }
    currentWorkingSensors.clear();
}

void Serializer::setSensorData(qint16 *data, quint64 timestamp)
{
    Sensor* sensor = qobject_cast<Sensor*>(sender());
    if (sensor == nullptr )
        return;
    if (sensor->currentStatus()== Sensor::TERMINATED)
        return;
    if (data == nullptr)
        return;
    emit sensorDataChanged(data, sensor->identifier());
}

void Serializer::setServiceData(Sensor::ServiceData data)
{
    Sensor* sensor = qobject_cast<Sensor*>(sender());
    if (sensor == nullptr )
        return;
    if (sensor->currentStatus()== Sensor::TERMINATED)
        return;
    emit serviceDataChanged(data, sensor->identifier());
}

void Serializer::setSensorStatus(Sensor::SensorStatus status)
{
    Sensor* sensor = qobject_cast<Sensor*>(sender());
    if (sensor == nullptr )
        return;
    emit sensorStatusChanged(status, sensor->identifier());
}

void Serializer::setSensorPose(QQuaternion pose)
{
    SensorGeometry* sg = qobject_cast<SensorGeometry*>(sender());
    if (sg == nullptr )
        return;

    emit sensorPoseChanged(pose, sg->identifier());
}

void Serializer::changeConfigurationByUser(QList<Sensor *> sensors)
{
    currentConfiguration = sensors;
    emit configurationChanged(currentConfiguration); //back-to-back configuration to mainwindow to apply changes in other elements e.g. combobox etc.
}

void Serializer::SaveConfig(QString path)
{
    qDebug() << "Saving config at" << path << "...";
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement(rootName);

        for (int row = 0; row < currentConfiguration.count(); row++)
        {
            xmlWriter.writeStartElement(childrenName);
            xmlWriter.writeAttribute(childrenAttributeName, QString::number(row));
            xmlWriter.writeTextElement(chilrenFields[1], currentConfiguration.at(row)->identifier());
            xmlWriter.writeTextElement(chilrenFields[2], currentConfiguration.at(row)->name());
            xmlWriter.writeTextElement(chilrenFields[3], QString::number(currentConfiguration.at(row)->baudrate()));
            xmlWriter.writeEndElement();
        }
        xmlWriter.writeEndDocument();
        file.close();
        qDebug() << "Successfully saved config";
    }
    else
    {
        qDebug() << "Save error - cannot open file for write.";
    }
}

void Serializer::LoadConfig(QString path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open configuration for read";
    }
    else
    {
        currentConfiguration.clear();
        QXmlStreamReader reader(&file);
        parseConfig(&reader, &currentConfiguration);
        emit configurationChanged(currentConfiguration);
        file.close();
    }
}

void Serializer::parseConfig(QXmlStreamReader* reader, QList<Sensor*>* configuration)
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
                addDeviceConfig(reader, configuration, &deviceCount);
        }
    }
    qDebug() << "Parsing completed.";
}

void Serializer::addDeviceConfig(QXmlStreamReader* reader, QList<Sensor*>* configuration, int *deviceCount)
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
            if (!addDevice(reader, configuration))
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

bool Serializer::addDevice(QXmlStreamReader* reader, QList<Sensor*>* configuration)
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
