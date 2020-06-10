#ifndef GEOMETRYESTIMATION_H
#define GEOMETRYESTIMATION_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QMatrix3x3>
#include <QVector3D>

class GeometryEstimation : public QObject
{
    Q_OBJECT
public:
    explicit GeometryEstimation(QObject *parent = nullptr);

private:
    const double eye[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    QMatrix3x3 Mprimary;
    QMatrix3x3 Msecondary;
    QMatrix3x3 MagCor;
    QVector3D GyrCor;

    void begin();
    void calibrateGyro();

    bool isReady = false;
    bool isGyroCalibrated = false;
    bool isMagnetCalibrated = false;

public slots:
    void GetPose(qint16 *buf, quint64 dt);

signals:
    void poseChanged(QMatrix3x3 rm);

};

#endif // GEOMETRYESTIMATION_H
