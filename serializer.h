
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include "mainwindow.h"

class Serializer : public QObject
{
    Q_OBJECT

public:
    explicit Serializer(QObject *parent = nullptr, MainWindow *mainwindow = nullptr);
    Serializer(MainWindow *mainwindow);
    QList<QSerialPortInfo> GetAvailablePorts();
    ~Serializer() {};

private:
    MainWindow* ui;

};

#endif
