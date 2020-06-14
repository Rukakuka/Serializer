
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QMap>
#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QXmlStreamWriter>
#include <QTableView>
#include <QStandardItemModel>
#include <QDir>
#include <QUrl>
#include <QMatrix3x3>

#include "sensor.h"
#include "mainwindow.h"
#include "sensorgeometry.h"
#include "filemanager.h"

class Serializer : public QObject
{
    Q_OBJECT

public:
    Serializer(MainWindow *mainwindow);
    ~Serializer() {};


public slots:
    void Begin();
    void Stop();
    void SaveConfiguration(QString path);
    void LoadConfiguration(QString path);
    void changeConfigurationByUser(QList<Sensor*> sensors);
    void BeginCalibration(QString identifier);
    void StopCalibration(QString identifier);
    void SaveCalibration(QString path);
    void LoadCalibration(QString path);

private:
    QString whatConnectedPort(QList<QSerialPortInfo> portlist, Sensor *sensor);
    QList<Sensor*> currentConfiguration;
    QList<Sensor*> currentWorkingSensors;
    QList<SensorGeometry*> currentWorkingSensorGeometries;

    bool validatePortName(QString portname);

private slots:
    void setSensorData(qint16* data, quint64 timestamp);
    void setServiceData(Sensor::ServiceData data);
    void setSensorStatus(Sensor::SensorStatus status);
    void setSensorPose(QQuaternion pose);
    void setCalibrationData(QVector3D *point);

signals:
    void stopSerial();
    void beginSerial();
    void configurationChanged(QList<Sensor *> sensors);
    void sensorDataChanged(qint16* data, QString identifier);
    void serviceDataChanged(Sensor::ServiceData data, QString identifier);
    void sensorStatusChanged(Sensor::SensorStatus status, QString identifier);
    void sensorPoseChanged(QQuaternion pose, QString identifier);
    void sensorCalibrationDataChanged(QVector3D* point, QString identifier);
    void stopCalibration(QString identifier);
    void beginCalibration(QString identifier);
    void stopMagnetometerCalibration();
};

#endif
