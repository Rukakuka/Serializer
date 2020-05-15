#include "serializer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Serializer ser;
    ser.show();
    return app.exec();
}
