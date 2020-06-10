#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QtWidgets>
#include <QQuaternion>
#include <QGenericMatrix>
#include <QVector3D>

class GeometryEngine : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GeometryEngine(QWidget *parent = 0);

    void setRotation(QMatrix3x3 rm);
    QQuaternion rotation;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

signals:

public slots:

private:
    float xrot,yrot,zrot;
    QMatrix4x4 rm4;
};

#endif // GEOMETRYENGINE_H
