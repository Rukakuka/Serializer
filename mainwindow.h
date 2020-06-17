#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sensor.h"
#include "sensorgeometry.h"
#include "logger.h"

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QApplication>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCoreApplication>
#include <QHeaderView>
#include <QFileDialog>

#include <QMatrix3x3>
#include <QtDataVisualization>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCustom3DItem>
#include <QImage>
#include <QObject>
#include <QVector3D>
#include <Q3DScatter>

QT_BEGIN_NAMESPACE
namespace Ui { class Mainwindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    enum CalibrationMode
    {
        NewCalibration,
        LoadFromFile
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::Mainwindow *ui;
    QList<QLineEdit*> *lineEditList;
    int whatColumnNumber(QString name);    
    QList<Sensor*> formatConfig(QTableWidget *table);

    QtDataVisualization::QCustom3DItem *sensorObject;
    QtDataVisualization::Q3DScatter *calibScatter;

    void initializeDrawingPlot();
    void initializeCalibrationPlot();

public slots:
    void setSensorData(qint16 *databuf, QString identifier);
    void setServiceData(Sensor::ServiceData data, QString identifier);
    void setSensorStatus(Sensor::SensorStatus status, QString identifier);
    void setConfigurationTable(QList<Sensor*> sensors);
    void setSensorPose(QQuaternion q, QString identifier);
    void addPointToScatter(QVector3D* point, QString identifier);
    void setCalibrationData(SensorGeometry::CalibrationData *data, QString identifier);

private slots:
    void on_btnStartStopSwitch_clicked();
    void on_btnLoadConfig_clicked();
    void on_btnSaveConfig_clicked();
    void on_btnAddDevice_clicked();
    void on_btnRemoveDevice_clicked();
    void on_comboSelectPort_currentIndexChanged(int index);
    void on_btnStartStopCalibration_clicked();
    void on_btnSaveCalibration_clicked();
    void on_btnLoadCalibration_clicked();

signals:
    void stopSerial();
    void beginSerial();
    void saveConfig(QString path);
    void loadConfig(QString path);
    void saveCalibration(QString path);
    void loadCalibration(QString path);
    void configurationChangedByUser(QList<Sensor*> sensors);
    void beginCalibration(QString identifier, MainWindow::CalibrationMode mode);
    void stopCalibration(QString identifier);

};

Q_DECLARE_METATYPE(MainWindow::CalibrationMode);
#endif // SERIALIZER_H
