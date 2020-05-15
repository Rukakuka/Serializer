#include "serializer.h"
#include "ui_serializer.h"

Serializer::Serializer(QWidget *parent) : QMainWindow(parent), ui(new Ui::Serializer)
{
    ui->setupUi(this);
}

Serializer::~Serializer()
{
    delete ui;
}

