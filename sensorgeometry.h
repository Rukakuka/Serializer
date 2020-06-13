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
    explicit SensorGeometry(QObject *parent = nullptr);
    SensorGeometry(QString identifier);
    QString identifier() { return m_identifier; }
private:
    const float eye[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    QQuaternion Qgyro;
    QQuaternion Qini;

    QVector3D GyrCor;    

    void begin();
    void calibrateGyro();

    QString m_identifier;
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
    void calculateNewPose(qint16 *buf, quint64 timestamp);

signals:
    void poseChanged(QQuaternion q);

};

#endif // GEOMETRYESTIMATION_H
