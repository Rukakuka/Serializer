#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Serializer; }
QT_END_NAMESPACE

class Serializer : public QMainWindow
{
    Q_OBJECT

public:
    Serializer(QWidget *parent = nullptr);
    ~Serializer();

private:
    Ui::Serializer *ui;
};
#endif // SERIALIZER_H
