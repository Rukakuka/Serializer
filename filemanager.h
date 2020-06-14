#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QFile>
#include <QXmlStreamWriter>

#include "sensor.h"

class FileManager : public QObject
{
    Q_OBJECT


public:
    static bool Save(QString path, QList<Sensor*> *configuration);
    static bool Load(QString path, QList<Sensor*> *configuration);

private:
    explicit FileManager(QObject *parent = nullptr);
    static bool config_parseFields(QXmlStreamReader *reader, QList<Sensor*> *configuration);
    static void config_parseDevice(QXmlStreamReader *reader, QList<Sensor*> *configuration, int *deviceCount);
    static void config_parse(QXmlStreamReader *reader, QList<Sensor*> *configuration);

signals:

};

#endif // FILEMANAGER_H
