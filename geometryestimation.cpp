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

    magnetCalibrated = true; // TODO : calibration engine

    if (buf == nullptr)
        return;

    if (gyroCalibCounter <= 50)
    {
        GyrCor.setX(GyrCor.x() + (float)buf[3]);
        GyrCor.setY(GyrCor.y() + (float)buf[4]);
        GyrCor.setZ(GyrCor.z() + (float)buf[5]);
        gyroCalibCounter++;
        prevTimestamp = timestamp;
        return;
    }
    else if(!gyroCalibrated)
    {
        gyroCalibrated = true;
        GyrCor.setX(GyrCor.x()/50);
        GyrCor.setY(GyrCor.y()/50);
        GyrCor.setZ(GyrCor.z()/50);
    }

    QVector3D acc(buf[0], buf[1], buf[2]);
    QVector3D mag(buf[5], buf[7], buf[8]);
    QVector3D gyr(buf[3], buf[4], buf[5]);

    gyr.setX(((0.0012207*(gyr.x() - GyrCor.x())) + gyrPrev.x())/2);
    gyr.setY(((0.0012207*(gyr.y() - GyrCor.y())) + gyrPrev.y())/2);
    gyr.setZ(((0.0012207*(gyr.z() - GyrCor.z())) + gyrPrev.z())/2);

    gyrPrev.setX(gyr.x());
    gyrPrev.setY(gyr.y());
    gyrPrev.setZ(gyr.z());

    float dt = ((float)timestamp - prevTimestamp)/1e6;
    prevTimestamp = timestamp;

    QVector3D vspeed(gyr.x()*dt, -gyr.z()*dt, -gyr.y()*dt);
    QQuaternion qspeed = gyro2quat(vspeed);

    Qgyro = Qgyro * qspeed;

    QVector3D Y = QVector3D::crossProduct(acc, mag);
    QVector3D X = QVector3D::crossProduct(Y, acc);

    Y.normalize();
    X.normalize();
    acc.normalize();

    if (!initialPoseCaptured)
    {
        Qini = QQuaternion::fromAxes(X, Y, acc).inverted();
        initialPoseCaptured = true;
    }
    QQuaternion qref = QQuaternion::fromAxes(X, Y, acc) * Qini;

    //Qgyro = QQuaternion::nlerp(qref, Qgyro, 0.999);

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

QQuaternion GeometryEstimation::gyro2quat(QVector3D gyro)
{
    float theta = gyro.length();
    gyro.normalize();
    QQuaternion q;
    float s = qSin(theta/2);
    q.setVector(s*gyro.x(), s*gyro.y(), s*gyro.z());
    q.setScalar(qCos(theta/2));
    return q;
}
