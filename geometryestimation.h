#ifndef GEOMETRYESTIMATION_H
#define GEOMETRYESTIMATION_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QMatrix3x3>
#include <QVector3D>
#include <QQuaternion>
#include <QtMath>

class GeometryEstimation : public QObject
{
    Q_OBJECT
public:
    explicit GeometryEstimation(QObject *parent = nullptr);

private:
    const float eye[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    QQuaternion Qgyro;
    QVector3D GyrCor;
    QQuaternion Qini;

    void begin();
    void calibrateGyro();

    bool isReady = false;
    bool gyroCalibrated = false;
    bool magnetCalibrated = false;
    bool initialPoseCaptured = false;

    int gyroCalibCounter;
    quint64 prevTimestamp;
    QVector3D gyrPrev;

    QMatrix3x3 angle2dcm(QVector3D gyro);    
    QQuaternion gyro2quat(QVector3D gyro);

public slots:
    void GetPose(qint16 *buf, quint64 timestamp);

signals:
    void poseChanged(QQuaternion q);

};

#endif // GEOMETRYESTIMATION_H
