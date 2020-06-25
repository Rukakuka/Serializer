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
    struct CalibrationData
    {
        QList<QVector3D*>* rawData;
        QList<QVector3D*>* calibratedData;
        QVector3D max;
        QVector3D min;
        QMatrix3x3 matrix;
        QVector3D bias;
    };

private:
    explicit SensorGeometry(QObject *parent = nullptr);

    CalibrationData magCalibrationData;

    void begin();
    void setMinMax(QVector3D *point);
    void calibrateGyro(qint16 *buf);
    void performCalibration();

    QQuaternion Qgyro;
    QQuaternion Qini;
    QVector3D GyrCor;    

    QString m_identifier;
    bool ready = false;
    bool gyroCalibrated = false;
    bool magnetCalibrated = false;
    bool initialPoseCaptured = false;

    int gyroCalibCounter;
    int magnetCalibCounter;

    quint64 prevTimestamp;
    QVector3D gyrPrev;   

    QVector3D rotateVector(QVector3D *point, QMatrix3x3 *rm);
    QVector3D stretchVector(QVector3D *point, QVector3D *k);
    QMatrix3x3 angle2dcm(QVector3D angles);
    QQuaternion gyro2quat(QVector3D gyro);

public slots:
    void calculateNewPose(qint16 *buf, quint64 timestamp);
    void setSingleCalibrationPoint(qint16 *buf, quint64 timestamp);
    void setLoadedRawData(QList<QVector3D*>* loadedData);
    void stopMagnetometerCalibration();

signals:
    void poseChanged(QQuaternion q);
    void sendSingleMagnetMeasure(QVector3D* v);
    void sendMagnetCalibratedMeasurements(SensorGeometry::CalibrationData *data);

};

Q_DECLARE_METATYPE(SensorGeometry::CalibrationData);
#endif // GEOMETRYESTIMATION_H
