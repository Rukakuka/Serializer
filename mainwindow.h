#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class Mainwindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::Mainwindow *ui;

public slots:
    void SetDataLabels(qint16 *databuf);
    void SetElapsedLabel(qint64 nsecs);

private slots:
    void on_btnStart_clicked();

signals:
    void mysignal();

};
#endif // SERIALIZER_H
