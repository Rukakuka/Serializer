#ifndef SENSORVISUALIZATION_H
#define SENSORVISUALIZATION_H

#include <QtWidgets>

class SensorVisualization : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit SensorVisualization(QWidget *parent = 0);

    void setRotation(float x, float y, float z);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

signals:

public slots:

private:
    float xrot,yrot,zrot;

};

#endif // SENSORVISUALIZATION_H
