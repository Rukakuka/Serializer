
#include "serializer.h"

Serializer::Serializer(MainWindow *mainwindow)
{
    QObject::connect(this, &Serializer::configurationChanged, mainwindow, &MainWindow::setConfigurationTable);
    QObject::connect(this, &Serializer::sensorDataChanged, mainwindow, &MainWindow::setSensorData);
    QObject::connect(this, &Serializer::serviceDataChanged, mainwindow, &MainWindow::setServiceData);
    QObject::connect(this, &Serializer::sensorStatusChanged, mainwindow, &MainWindow::setSensorStatus);
    QObject::connect(this, &Serializer::sensorPoseChanged, mainwindow, &MainWindow::setSensorPose);
    QObject::connect(this, &Serializer::sensorSingleCalibrationMeasurement, mainwindow, &MainWindow::addSingleCalibrationMeasurement);
    QObject::connect(this, &Serializer::sensorCalibrationDataChanged, mainwindow, &MainWindow::setCalibrationData);

    QObject::connect(mainwindow, &MainWindow::saveConfig, this, &Serializer::SaveConfiguration);
    QObject::connect(mainwindow, &MainWindow::loadConfig, this, &Serializer::LoadConfiguration);
    QObject::connect(mainwindow, &MainWindow::beginSerial, this, &Serializer::Begin);
    QObject::connect(mainwindow, &MainWindow::stopSerial, this, &Serializer::Stop);
    QObject::connect(mainwindow, &MainWindow::configurationChangedByUser, this, &Serializer::changeConfigurationByUser);

    QObject::connect(mainwindow, &MainWindow::saveCalibration, this, &Serializer::SaveCalibration);
    QObject::connect(mainwindow, &MainWindow::loadCalibration, this, &Serializer::LoadCalibration);
    QObject::connect(mainwindow, &MainWindow::beginCalibration, this, &Serializer::BeginCalibration);
    QObject::connect(mainwindow, &MainWindow::stopCalibration, this, &Serializer::StopCalibration);

}

void Serializer::Begin()
{    
    QList<QSerialPortInfo> portlist = QSerialPortInfo::availablePorts();
    if (currentConfiguration.isEmpty())
    {
        LoadConfiguration("E:/QtProjects/Serializer/configuration.xml");
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
            QObject::connect(sensor, &Sensor::threadTerminating, sg, &SensorGeometry::deleteLater);

            // outcoming data connections
            QObject::connect(sensor, &Sensor::sensorDataChanged, this, &Serializer::setSensorData);
            QObject::connect(sensor, &Sensor::serviceDataChanged, this, &Serializer::setServiceData);
            QObject::connect(sensor, &Sensor::statusChanged, this, &Serializer::setSensorStatus);
            QObject::connect(sensor, &Sensor::sensorDataChanged, sg, &SensorGeometry::calculateNewPose);
            QObject::connect(sg, &SensorGeometry::poseChanged, this, &Serializer::setSensorPose);

            qDebug() << "Sensor " << sensor->name() << " added";
            currentWorkingSensors.append(sensor);
            currentWorkingSensorGeometries.append(sg);

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
    if (portname.startsWith("COM", Qt::CaseSensitive) && portname.mid(3).toInt() > 0)
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
    for (int i = 0; i < currentWorkingSensors.size(); i++)
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
    currentWorkingSensorGeometries.clear();
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

void Serializer::setSingleCalibrationPoint(QVector3D* point)
{
    SensorGeometry* sg = qobject_cast<SensorGeometry*>(sender());
    if (sg == nullptr )
        return;
    emit sensorSingleCalibrationMeasurement(point, sg->identifier());
}

void Serializer::changeConfigurationByUser(QList<Sensor *> sensors)
{
    currentConfiguration = sensors;
    emit configurationChanged(currentConfiguration); //back-to-back configuration to mainwindow to apply changes in other elements e.g. combobox etc.
}

void Serializer::SaveConfiguration(QString path)
{
    qDebug() << "Saving serial config at" << path << "...";
    if (FileManager::Save(path, &currentConfiguration))
    {
        qDebug() << "Successfully saved config";
    }
    else
    {
        qDebug() << "Save error - cannot open file for write.";
    }
}

void Serializer::LoadConfiguration(QString path)
{
    if (FileManager::Load(path, &currentConfiguration))
    {
        emit configurationChanged(currentConfiguration);
    }
    else
    {
        qDebug() << "Cannot open configuration file  for read";
    }
}

void Serializer::SaveCalibration(QString path)
{
    qDebug() << "Dummy : save calibration at" << path;
}

void Serializer::LoadCalibration(QString path)
{
    qDebug() << "Dummy : load calibration from" << path;
}

void Serializer::SetMagnetCalibratedMeasurements(SensorGeometry::CalibrationData *data)
{
    SensorGeometry* sg = qobject_cast<SensorGeometry*>(sender());
    if (sg == nullptr )
        return;
    emit sensorCalibrationDataChanged(data, sg->identifier());
}

void Serializer::BeginCalibration(QString identifier)
{
    SensorGeometry *sg;
    foreach(SensorGeometry* geometry, currentWorkingSensorGeometries)
    {
        if (geometry->identifier() == identifier)
        {
            sg = geometry;
            break;
        }
    }
    if (sg == nullptr)
    {
        qDebug() << "No SensorGeometry object found. Calibration start cancelled";
        return;
    }
    Sensor *sensor;
    foreach(Sensor* s, currentWorkingSensors)
    {
        if (s->identifier() == sg->identifier())
        {
            sensor = s;
            break;
        }
    }
    if (sensor == nullptr)
    {
        qDebug() << "No Sensor object found. Calibration start cancelled";
        return;
    }
    qDebug() << "Magnetometer calibration begin for" << sensor->name();

    QObject::connect(sensor, &Sensor::sensorDataChanged, sg, &SensorGeometry::calibrateMagnetometer);
    QObject::connect(sg, &SensorGeometry::sendSingleMagnetMeasure, this, &Serializer::setSingleCalibrationPoint, Qt::UniqueConnection);
    QObject::connect(this, &Serializer::stopMagnetometerCalibration, sg, &SensorGeometry::stopMagnetometerCalibration, Qt::UniqueConnection);
    QObject::connect(sg, &SensorGeometry::sendMagnetCalibratedMeasurements, this, &Serializer::SetMagnetCalibratedMeasurements, Qt::UniqueConnection);
}

void Serializer::StopCalibration(QString identifier)
{
    SensorGeometry *sg;
    foreach(SensorGeometry* geometry, currentWorkingSensorGeometries)
    {
        if (geometry->identifier() == identifier)
        {
            sg = geometry;
            break;
        }
    }
    if (sg == nullptr)
    {
        qDebug() << "No SensorGeometry object found. Calibration stop cancelled";
        return;
    }
    Sensor *sensor;
    foreach(Sensor* s, currentWorkingSensors)
    {
        if (s->identifier() == sg->identifier())
        {
            sensor = s;
            break;
        }
    }
    if (sensor == nullptr)
    {
        qDebug() << "No Sensor object found. Calibration stop cancelled";
        return;
    }
    emit stopMagnetometerCalibration();

    QObject::disconnect(sensor, &Sensor::sensorDataChanged, sg, &SensorGeometry::calibrateMagnetometer);

    qDebug() << "Magnetormter calibration done for" << sensor->name();
}
