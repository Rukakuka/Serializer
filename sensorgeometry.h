#ifndef GEOMETRYESTIMATION_H
#define GEOMETRYESTIMATION_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QMatrix3x3>
#include <QVector3D>
#include <QQuaternion>
#include <QtMath>

class SensorGeometry : public QObject
{
    Q_OBJECT
public:

    SensorGeometry(QString identifier);
    QString identifier() { return m_identifier; }

private:
    explicit SensorGeometry(QObject *parent = nullptr);

    void begin();
    void calibrateGyro(qint16 *buf);

    QQuaternion Qgyro;
    QQuaternion Qini;
    QVector3D GyrCor;    

    QString m_identifier;
    bool ready = false;
    bool gyroCalibrated = false;
    bool magnetCalibrated = false;
    bool initialPoseCaptured = false;
    bool magnetCalibrationFinished = false;

    int gyroCalibCounter;
    int magnetCalibCounter;

    quint64 prevTimestamp;
    QVector3D gyrPrev;
    QList<QVector3D*> magCalibrationData;

    QMatrix3x3 angle2dcm(QVector3D gyro);
    QQuaternion gyro2quat(QVector3D gyro);

public slots:
    void calculateNewPose(qint16 *buf, quint64 timestamp);
    void calibrateMagnetometer(qint16 *buf, quint64 timestamp);
    void stopMagnetometerCalibration();

signals:
    void poseChanged(QQuaternion q);
    void calibrationDataChanged(QVector3D* v);

};

#endif // GEOMETRYESTIMATION_H
