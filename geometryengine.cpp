#include "geometryengine.h"


GeometryEngine::GeometryEngine(QWidget *parent) : QOpenGLWidget(parent)
{
    xrot = yrot = zrot = 0.0;
}

void GeometryEngine::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1, 1, 1, 1);

    f->glEnable(GL_LIGHTING);
    f->glEnable(GL_LIGHT0);
}

void GeometryEngine::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0, 0, -7.0);
    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
    glRotatef(zrot, 0.0, 0.0, 1.0);


    glBegin(GL_QUADS);

    glNormal3d(0,0,+1);

    glVertex3d(-1,-1,0.1f);
    glVertex3d(-1,1,0.1f);
    glVertex3d(1,1,0.1f);
    glVertex3d(1,-1,0.1f);

    glEnd();
}

void GeometryEngine::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void GeometryEngine::setRotation(float x, float y, float z)
{
    xrot = x;
    yrot = y;
    zrot = z;
    update();
}
