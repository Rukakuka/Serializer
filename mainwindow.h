#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sensor.h"
#include "serializer.h"

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

QT_BEGIN_NAMESPACE
namespace Ui { class Mainwindow; }
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QTableWidget*)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Serializer *serializer = nullptr);
    ~MainWindow();

private:
    Ui::Mainwindow *ui;
    Serializer *serializer;
    QList<QLineEdit*> *lineEditList;

public slots:
    void SetDataLabels(qint16 *databuf);
    void SetServiceData(Sensor::ServiceData *sd);
    void SetTableCurrentPorts(QList<Sensor*>* ports);

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void on_btnTerminate_clicked();
    void on_btnLoadConfig_clicked();
    void on_btnSaveConfig_clicked();
    void on_btnAddDevice_clicked();
    void on_btnRemoveDevice_clicked();

signals:
    void stopSerial();
    void beginSerial();
    void terminateSerial();
    void saveConfig(QTableWidget*, QString);
    void loadConfig(QString);
};
#endif // SERIALIZER_H
