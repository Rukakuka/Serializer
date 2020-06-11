#include "geometryestimation.h"



GeometryEstimation::GeometryEstimation(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QQuaternion>();
    begin();
}

void GeometryEstimation::begin()
{

    Qgyro.setScalar(1);
    Qgyro.setVector(QVector3D(0,0,0));

    GyrCor.setX(0);
    GyrCor.setY(0);
    GyrCor.setZ(0);

    gyrPrev.setX(0);
    gyrPrev.setY(0);
    gyrPrev.setZ(0);

    gyroCalibCounter = 0;
    prevTimestamp = 0;

    isReady = true;
}

void GeometryEstimation::calibrateGyro()
{

}


void GeometryEstimation::GetPose(qint16 *buf, quint64 timestamp)
{

    if (!isReady)
    {
        begin();
    }

    isMagnetCalibrated = true; // TODO : calibration engine

    if (buf == nullptr)
        return;


    if (gyroCalibCounter <= 500)
    {
        GyrCor.setX(GyrCor.x() + (float)buf[3]);
        GyrCor.setY(GyrCor.y() + (float)buf[4]);
        GyrCor.setZ(GyrCor.z() + (float)buf[5]);
        gyroCalibCounter++;
        prevTimestamp = timestamp;
        return;
    }
    else if(!isGyroCalibrated)
    {
        isGyroCalibrated = true;
        GyrCor.setX(GyrCor.x()/500);
        GyrCor.setY(GyrCor.y()/500);
        GyrCor.setZ(GyrCor.z()/500);
        qDebug() << GyrCor.x() << GyrCor.y() << GyrCor.z();
    }

    //QVector3D acc(buf[0], buf[1], buf[2]);
    //QVector3D mag(-buf[7], buf[6], buf[8]);
    QVector3D gyr(buf[3], buf[4], buf[5]);

    gyr.setX(((0.00112*(gyr.x() - GyrCor.x())) + gyrPrev.x())/2);
    gyr.setY(((0.00112*(gyr.y() - GyrCor.y())) + gyrPrev.y())/2);
    gyr.setZ(((0.00112*(gyr.z() - GyrCor.z())) + gyrPrev.z())/2);

    gyrPrev.setX(gyr.x());
    gyrPrev.setY(gyr.y());
    gyrPrev.setZ(gyr.z());

    float dt = ((float)timestamp - prevTimestamp)/1e6;
    prevTimestamp = timestamp;

    QVector3D vspeed(-gyr.x()*dt, gyr.z()*dt, gyr.y()*dt);

    QQuaternion qspeed = QQuaternion::fromRotationMatrix(angle2dcm(vspeed).transposed());
    Qgyro.normalize();
    qspeed.normalize();
    Qgyro = qspeed * Qgyro;

    /*
    QVector3D A = QVector3D::crossProduct(acc, mag);
    QVector3D B = QVector3D::crossProduct(A, acc);

    A.normalize();
    B.normalize();
    acc.normalize();

    float val[] = {B.x(), B.y(), B.z(), A.x(), A.y(), A.z(), acc.x(), acc.y(), acc.z()};

    QMatrix3x3 Mcorrection(val);
    */

    //for (int row = 0; row < 3; row++)
      //  for (int col = 0; col < 3; col++)
        //    qDebug() << Mcorrection(row,col);
    emit poseChanged(Qgyro);
}


QMatrix3x3 GeometryEstimation::angle2dcm(QVector3D gyro)
{
    QMatrix3x3 dcm(eye);
    QVector<float> c(3);
    QVector<float> s(3);
    QVector<float> V = {gyro.x(), gyro.y(), gyro.z()};

    for (int i =0; i<3; i++)
    {
        c[i] = qCos(V[i]);
        s[i] = qSin(V[i]);
    }
    // XYZ
    // [  cy*cz   sz*cx+sy*sx*cz   sz*sx-sy*cx*cz]
    // [ -cy*sz   cz*cx-sy*sx*sz   cz*sx+sy*cx*sz]
    // [     sy           -cy*sx            cy*cx]
    dcm(0,0)= c[1]*c[2];     dcm(0,1)=s[2]*c[0]+s[1]*s[0]*c[2];     dcm(0,2)=s[2]*s[0]-s[1]*c[0]*c[2];
    dcm(1,0)=-c[1]*s[2];     dcm(1,1)=c[2]*c[0]-s[1]*s[0]*s[2];     dcm(1,2)=c[2]*s[0]+s[1]*c[0]*s[2];
    dcm(2,0)=      s[1];     dcm(2,1)=              -c[1]*s[0];     dcm(2,2)=               c[1]*c[0];

    return dcm;
}
