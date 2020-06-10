#include "geometryestimation.h"



GeometryEstimation::GeometryEstimation(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QMatrix3x3>();
    begin();
}

void GeometryEstimation::begin()
{

    Mprimary.setToIdentity();
    Msecondary.setToIdentity();
    MagCor.setToIdentity();

    GyrCor.setX(0);
    GyrCor.setY(0);
    GyrCor.setZ(0);

    isReady = true;
}

void GeometryEstimation::calibrateGyro()
{

}


void GeometryEstimation::GetPose(qint16 *buf, quint64 dt)
{
    if (!isReady)
    {
        begin();
    }

    isMagnetCalibrated = true; // TODO : calibration engine

    if (buf == nullptr)
        return;

    float gx = buf[3];
    float gy = buf[4];
    float gz = buf[5];

    QVector3D acc(buf[0], buf[1], buf[2]);
    QVector3D mag(-buf[7], buf[6], buf[8]);
    QVector3D A = QVector3D::crossProduct(acc, mag);
    QVector3D B = QVector3D::crossProduct(A, acc);

    A.normalize();
    B.normalize();
    acc.normalize();

    float val[] = {B.x(), B.y(), B.z(), A.x(), A.y(), A.z(), acc.x(), acc.y(), acc.z()};

    QMatrix3x3 Mcorrection(val);
    //for (int row = 0; row < 3; row++)
      //  for (int col = 0; col < 3; col++)
        //    qDebug() << Mcorrection(row,col);

    emit poseChanged(Mcorrection);
}

