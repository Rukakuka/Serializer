#ifndef GEOMETRYESTIMATION_H
#define GEOMETRYESTIMATION_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QGenericMatrix>
#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>
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

    struct CalibrationData
    {
        QList<QVector3D*> rawData;
        QVector3D max;
        QVector3D min;
        QMatrix3x3 matrix;
        QVector3D bias;
    } magCalibrationData;

    void begin();
    void calibrateGyro(qint16 *buf);
    QMatrix3x3 getSoftIronMatrix(QVector3D bias);
    QVector3D getBias();
    void getHardIron(QMatrix3x3 softIron, QVector3D bias);
    QVector3D rotate(QVector3D point, QMatrix3x3 rm);
    void setMinMax(QVector3D *point);

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

    QMatrix3x3 angle2dcm(QVector3D gyro);
    QQuaternion gyro2quat(QVector3D gyro);



public slots:
    void calculateNewPose(qint16 *buf, quint64 timestamp);
    void calibrateMagnetometer(qint16 *buf, quint64 timestamp);
    void stopMagnetometerCalibration();

signals:
    void poseChanged(QQuaternion q);
    void sendSingleMagnetMeasure(QVector3D* v);

};

#endif // GEOMETRYESTIMATION_H
