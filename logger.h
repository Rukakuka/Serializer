#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QDebug>
#include <QDateTime>

#include "mainwindow.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    //explicit Logger(QObject *parent = nullptr, MainWindow *window = nullptr);
    void message(QString *text);
    //void initialize(MainWindow *mw);
private:
    explicit Logger(QObject *parent = nullptr) {};
    //MainWindow *mainwindow;
    bool initialized;
signals:
    void setText();
};

#endif // LOGGER_H
