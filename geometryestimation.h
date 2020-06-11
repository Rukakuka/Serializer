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

    QMatrix3x3 Mgyro;
    QVector3D GyrCor;

    void begin();
    void calibrateGyro();

    bool isReady = false;
    bool isGyroCalibrated = false;
    bool isMagnetCalibrated = false;

    int gyroCalibCounter;
    quint64 prevTimestamp;
    QVector3D gyrPrev;

    QMatrix3x3 angle2dcm(QVector3D gyro);

public slots:
    void GetPose(qint16 *buf, quint64 timestamp);

signals:
    void poseChanged(QQuaternion q);

};

#endif // GEOMETRYESTIMATION_H
