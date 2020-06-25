#include "sensorgeometry.h"

SensorGeometry::SensorGeometry(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QQuaternion>();
    qRegisterMetaType<SensorGeometry::CalibrationData>();
    begin();
}

SensorGeometry::SensorGeometry(QString identifier)
{
    m_identifier = identifier;
}

void SensorGeometry::begin()
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

    magCalibrationData.max.setX(INT32_MIN);
    magCalibrationData.max.setY(INT32_MIN);
    magCalibrationData.max.setZ(INT32_MIN);
    magCalibrationData.min.setX(INT32_MAX);
    magCalibrationData.min.setY(INT32_MAX);
    magCalibrationData.min.setZ(INT32_MAX);

    magCalibrationData.calibratedData = new QList<QVector3D*>;
    magCalibrationData.rawData = new QList<QVector3D*>;

    // LOAD CALIBRATION DATA FROM FILE

    ready = true;
}

void SensorGeometry::calibrateGyro(qint16 *buf)
{
    if (gyroCalibCounter <= 50)
    {
        GyrCor.setX(GyrCor.x() + (float)buf[3]);
        GyrCor.setY(GyrCor.y() + (float)buf[4]);
        GyrCor.setZ(GyrCor.z() + (float)buf[5]);
        gyroCalibCounter++;
        return;
    }
    else if(!gyroCalibrated)
    {
        gyroCalibrated = true;
        GyrCor.setX(GyrCor.x()/50);
        GyrCor.setY(GyrCor.y()/50);
        GyrCor.setZ(GyrCor.z()/50);
    }
}

void SensorGeometry::calculateNewPose(qint16 *buf, quint64 timestamp)
{
    if (!ready)
        begin();

    if (buf == nullptr)
        return;

    //magnetCalibrated = true;

    if (!magnetCalibrated)
        return;

    if (!gyroCalibrated)
    {
        calibrateGyro(buf);
        prevTimestamp = timestamp;
        return;
    }

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

    QVector3D Z = QVector3D(buf[0], buf[1], buf[2]); // accelerometer axis
    QVector3D Y = QVector3D::crossProduct(Z, QVector3D(buf[5], buf[7], buf[8])); // accelerometer + magnetometer axis
    QVector3D X = QVector3D::crossProduct(Y, Z); // X axis perpendicular to both

    X.normalize();
    Y.normalize();    
    Z.normalize();

    if (!initialPoseCaptured)
    {
        Qini = QQuaternion::fromAxes(X, Y, Z).inverted();
        initialPoseCaptured = true;
    }
    QQuaternion qref = QQuaternion::fromAxes(X, Y, Z) * Qini;

    //Qgyro = QQuaternion::nlerp(qref, Qgyro, 0.999);

    emit poseChanged(Qgyro);
}

void SensorGeometry::setSingleCalibrationPoint(qint16 *buf, quint64 timestamp)
{
    if (buf == nullptr)
        return;

    if (!magnetCalibrated)
    {
        QVector3D *pointRaw = new QVector3D((float)buf[6], (float)buf[7], (float)buf[8]);
        if (!magCalibrationData.rawData->isEmpty())
        {

            if (!magCalibrationData.rawData->isEmpty() &&
                pointRaw->x() == magCalibrationData.rawData->last()->x() &&
                pointRaw->y() == magCalibrationData.rawData->last()->y() &&
                pointRaw->z() == magCalibrationData.rawData->last()->z())
            {
                delete pointRaw;
                return;
            }
            if (magCalibrationData.rawData->count() > 1)
            {

                if (qFabs(pointRaw->x() -  magCalibrationData.rawData->last()->x()) +
                    qFabs(pointRaw->y() -  magCalibrationData.rawData->last()->y()) +
                    qFabs(pointRaw->z() -  magCalibrationData.rawData->last()->z()) > 100) // delete pikes from measurements
                {
                    delete pointRaw;
                    return;
                }
            }
        }
        magCalibrationData.rawData->append(pointRaw);
        setMinMax(pointRaw);

        QVector3D *pointToBeCalibrated = new QVector3D(*pointRaw);
        magCalibrationData.calibratedData->append(pointToBeCalibrated);

        emit sendSingleMagnetMeasure(pointRaw);

        if (magCalibrationData.rawData->count() > 50000) // prevent UI lag too many points to render
        {
            stopMagnetometerCalibration();
        }
    }
}

void SensorGeometry::setLoadedRawData(QList<QVector3D *> *loadedData)
{
    if (loadedData == nullptr)
        return;
    magCalibrationData.calibratedData = loadedData;
    for (int i = 0; i < magCalibrationData.calibratedData->count(); i++)
    {
        QVector3D *point = magCalibrationData.calibratedData->at(i);
        setMinMax(point);
    }
    this->stopMagnetometerCalibration();
}

void SensorGeometry::stopMagnetometerCalibration()
{
    performCalibration();
    magnetCalibrated = true;
    emit sendMagnetCalibratedMeasurements(&magCalibrationData);

    /*
    while (!magCalibrationData.rawData.isEmpty())
    {
        delete magCalibrationData.rawData.last();
    }
    */

}

void SensorGeometry::performCalibration()
{
    if (magCalibrationData.calibratedData->count() > 0 &&
            magCalibrationData.min.x() < magCalibrationData.max.x() &&
            magCalibrationData.min.y() < magCalibrationData.max.y() &&
            magCalibrationData.min.z() < magCalibrationData.max.z())
    {
        QVector3D bias;
        bias.setX((magCalibrationData.max.x() + magCalibrationData.min.x())/2);
        bias.setY((magCalibrationData.max.y() + magCalibrationData.min.y())/2);
        bias.setZ((magCalibrationData.max.z() + magCalibrationData.min.z())/2);
        magCalibrationData.bias = bias;

        qDebug() << magCalibrationData.max.x();
        qDebug() << magCalibrationData.max.y();
        qDebug() << magCalibrationData.max.z();

        magCalibrationData.max.setX(INT32_MIN);
        magCalibrationData.max.setY(INT32_MIN);
        magCalibrationData.max.setZ(INT32_MIN);

        magCalibrationData.min.setX(INT32_MAX);
        magCalibrationData.min.setY(INT32_MAX);
        magCalibrationData.min.setZ(INT32_MAX);

        for (int i = 0; i < magCalibrationData.calibratedData->count(); i++)
        {
            QVector3D *point = magCalibrationData.calibratedData->at(i);
            point->setX(magCalibrationData.calibratedData->at(i)->x()-bias.x());
            point->setY(magCalibrationData.calibratedData->at(i)->y()-bias.y());
            point->setZ(magCalibrationData.calibratedData->at(i)->z()-bias.z());
            setMinMax(point);
        }

        qDebug() << magCalibrationData.max.x();
        qDebug() << magCalibrationData.max.y();
        qDebug() << magCalibrationData.max.z();

        float x = magCalibrationData.max.x();
        float y = magCalibrationData.max.y();
        float z = magCalibrationData.max.z();

        float alpha = qAtan2(y,x);//qAsin(y / qSqrt(qPow(x, 2) + qPow(y, 2)));
        float theta = qAtan2(z,y);//qAsin(z / qSqrt(qPow(y, 2) + qPow(z, 2)));
        float gamma = qAtan2(x,z);// / qSqrt(qPow(z, 2) + qPow(x, 2)));

        const float eye[9] = {1, 0, 0,
                              0, 1, 0,
                              0, 0, 1};

        const float pi = 3.14159265;

        qDebug() << alpha*180/pi;
        qDebug() << theta*180/pi;
        qDebug() << gamma*180/pi;

        QMatrix3x3 softIron = angle2dcm(QVector3D(alpha, theta, gamma)); //QQuaternion::fromEulerAngles(gamma*180/pi, alpha*180/pi, theta*180/pi).toRotationMatrix();

        qDebug() << softIron;

        magCalibrationData.max.setX(INT32_MIN);
        magCalibrationData.max.setY(INT32_MIN);
        magCalibrationData.max.setZ(INT32_MIN);

        magCalibrationData.min.setX(INT32_MAX);
        magCalibrationData.min.setY(INT32_MAX);
        magCalibrationData.min.setZ(INT32_MAX);

        for (int i = 0; i < magCalibrationData.calibratedData->count(); i++)
        {
            QVector3D point = rotateVector(magCalibrationData.calibratedData->at(i), &softIron);
            setMinMax(&point);
        }

        const float fnorm = 1000; // norm of magnetic field
        float kx = fnorm / ((magCalibrationData.max.x()-magCalibrationData.min.x())/2);
        float ky = fnorm / ((magCalibrationData.max.y()-magCalibrationData.min.y())/2);
        float kz = fnorm / ((magCalibrationData.max.z()-magCalibrationData.min.z())/2);

        QVector3D k(kx, ky, kz);
        qDebug() << k;
        qDebug() << magCalibrationData.max.x()-magCalibrationData.min.x();
        qDebug() << magCalibrationData.max.y()-magCalibrationData.min.y();
        qDebug() << magCalibrationData.max.z()-magCalibrationData.min.z();

        //fnorm / (magCalibrationData.max.y()-magCalibrationData.min.y()),
          //          fnorm / (magCalibrationData.max.z()-magCalibrationData.min.z()));

        magCalibrationData.matrix(0,0) = softIron(0,0)*k.x();
        magCalibrationData.matrix(1,0) = softIron(1,0)*k.x();
        magCalibrationData.matrix(2,0) = softIron(2,0)*k.x();

        magCalibrationData.matrix(0,1) = softIron(0,1)*k.y();
        magCalibrationData.matrix(1,1) = softIron(1,1)*k.y();
        magCalibrationData.matrix(2,1) = softIron(2,1)*k.y();

        magCalibrationData.matrix(0,2) = softIron(0,2)*k.z();
        magCalibrationData.matrix(1,2) = softIron(1,2)*k.z();
        magCalibrationData.matrix(2,2) = softIron(2,2)*k.z();

        /*
        for (int row = 0; row < 3; row++)
            magCalibrationData.matrix(row,0) = softIron(row,0)*k.x();
        for (int row = 0; row < 3; row++)
            magCalibrationData.matrix(row,1) = softIron(row,1)*k.y();
        for (int row = 0; row < 3; row++)
            magCalibrationData.matrix(row,2) = softIron(row,2)*k.z();
        */

        for (int i = 0; i < magCalibrationData.calibratedData->count(); i++)
        {
            QVector3D *point = magCalibrationData.calibratedData->at(i);
            *point = rotateVector(magCalibrationData.calibratedData->at(i), &magCalibrationData.matrix);
        }

        /*
        for (int i = 0; i < magCalibrationData.calibratedData->count(); i++)
        {
            *(magCalibrationData.calibratedData->at(i)) = stretchVector(magCalibrationData.calibratedData->at(i), &k);
        }
        */

        magnetCalibrated = true;
    }
    else
    {
        qDebug() << "Failed to calibrate magnetometer : bad raw data";
    }
}

void SensorGeometry::setMinMax(QVector3D *point)
{
    if (point->x() > magCalibrationData.max.x())
        magCalibrationData.max.setX(point->x());
    if (point->y() > magCalibrationData.max.y())
        magCalibrationData.max.setY(point->y());
    if (point->z() > magCalibrationData.max.z())
        magCalibrationData.max.setZ(point->z());

    if (point->x() < magCalibrationData.min.x())
        magCalibrationData.min.setX(point->x());
    if (point->y() < magCalibrationData.min.y())
        magCalibrationData.min.setY(point->y());
    if (point->z() < magCalibrationData.min.z())
        magCalibrationData.min.setZ(point->z());
}

QVector3D SensorGeometry::rotateVector(QVector3D *point, QMatrix3x3 *rm)
{
    /*
    qDebug() << (*rm)(0,0) << (*rm)(0,1) << (*rm)(0,2);
    qDebug() << (*rm)(1,0) << (*rm)(1,1) << (*rm)(1,2);
    qDebug() << (*rm)(2,0) << (*rm)(2,1) << (*rm)(2,2);
    */
    return QVector3D(point->x()*(*rm)(0,0) + point->y()*(*rm)(0,1) + point->z()*(*rm)(0,2),
                     point->x()*(*rm)(1,0) + point->y()*(*rm)(1,1) + point->z()*(*rm)(1,2),
                     point->x()*(*rm)(2,0) + point->y()*(*rm)(2,1) + point->z()*(*rm)(2,2));
}

QVector3D SensorGeometry::stretchVector(QVector3D *point, QVector3D *k)
{
    return QVector3D(point->x()*k->x(), point->y()*k->y(),point->z()*k->z());
}



QMatrix3x3 SensorGeometry::angle2dcm(QVector3D angles)
{
    const float eye[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    QMatrix3x3 dcm(eye);
    QVector<float> c(3);
    QVector<float> s(3);
    QVector<float> V = {angles.x(), angles.y(), angles.z()};

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

QQuaternion SensorGeometry::gyro2quat(QVector3D gyro)
{
    float theta = gyro.length();
    gyro.normalize();
    QQuaternion q;
    float s = qSin(theta/2);
    q.setVector(s*gyro.x(), s*gyro.y(), s*gyro.z());
    q.setScalar(qCos(theta/2));
    return q;
}
