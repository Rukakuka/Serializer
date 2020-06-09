#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QtWidgets>
#include <QQuaternion>

class GeometryEngine : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GeometryEngine(QWidget *parent = 0);

    void setRotation(float x, float y, float z);
    QQuaternion rotation;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

signals:

public slots:

private:
    float xrot,yrot,zrot;

};

#endif // GEOMETRYENGINE_H
