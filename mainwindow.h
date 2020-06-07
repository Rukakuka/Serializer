#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sensor.h"

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

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QScreen>

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>

QT_BEGIN_NAMESPACE
namespace Ui { class Mainwindow; }
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QTableWidget*)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::Mainwindow *ui;
    QList<QLineEdit*> *lineEditList;

    int whatColumnNumber(QString name);    
    QList<Sensor*> formatConfig(QTableWidget *table);

public slots:
    void SetSensorData(qint16 *databuf, QString identifier);
    void SetServiceData(Sensor::ServiceData data, QString identifier);
    void SetSensorStatus(Sensor::SensorStatus status, QString identifier);
    void SetConfigurationTable(QList<Sensor*> sensors);

private slots:
    void on_btnStartStopSwitch_clicked();
    void on_btnLoadConfig_clicked();
    void on_btnSaveConfig_clicked();
    void on_btnAddDevice_clicked();
    void on_btnRemoveDevice_clicked();
    void on_comboSelectPort_currentIndexChanged(int index);

signals:    
    void stopSerial();
    void beginSerial();
    void saveConfig(QString);
    void loadConfig(QString);
    void configurationChangedByUser(QList<Sensor*> sensors);

};
#endif // SERIALIZER_H
