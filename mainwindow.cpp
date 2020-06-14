#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "geometryengine.h"

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
    ui->setupUi(this);

    this->lineEditList = new QList<QLineEdit*>;

    lineEditList->append(ui->lineEditAx);
    lineEditList->append(ui->lineEditAy),
    lineEditList->append(ui->lineEditAz),
    lineEditList->append(ui->lineEditGx),
    lineEditList->append(ui->lineEditGy),
    lineEditList->append(ui->lineEditGz),
    lineEditList->append(ui->lineEditMx),
    lineEditList->append(ui->lineEditMy),
    lineEditList->append(ui->lineEditMz);

    ui->tableCurrentConfig->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->tableCurrentConfig->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    ui->btnStartStopSwitch->setText("Start");
    ui->btnStartStopCalibration->setText("Start calibration");
    ui->btnStartStopCalibration->setEnabled(false);

    initializeDrawingPlot();
    initializeCalibrationPlot();
}

void MainWindow::initializeDrawingPlot()
{
    using namespace QtDataVisualization;

    Q3DSurface *surface = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(surface);

    if (!surface->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return;
    }
    else
    {
        container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        container->setFocusPolicy(Qt::StrongFocus);

        QHBoxLayout *hLayout = new QHBoxLayout(ui->drawingPlot);
        QVBoxLayout *vLayout = new QVBoxLayout();
        hLayout->addWidget(container, 1);
        hLayout->addLayout(vLayout);
        vLayout->setAlignment(Qt::AlignTop);
    }

    surface->axisX()->setRange(-3.0f, 3.0f);
    surface->axisY()->setRange(-3.0f, 3.0f);
    surface->axisZ()->setRange(-3.0f, 3.0f);

    surface->axisX()->setTitle(QStringLiteral("X"));
    surface->axisY()->setTitle(QStringLiteral("Y"));
    surface->axisZ()->setTitle(QStringLiteral("Z"));

    surface->axisX()->setTitleVisible(true);
    surface->axisY()->setTitleVisible(true);
    surface->axisZ()->setTitleVisible(true);

    QImage color = QImage(2, 2, QImage::Format_RGB32);
    color.fill(Qt::red);

    this->sensorObject = new QCustom3DItem();
    sensorObject->setMeshFile("E:/QtProjects/Serializer/solids/brick.obj");

    sensorObject->setScalingAbsolute(true);
    sensorObject->setScaling(QVector3D(2, 2, 2));
    sensorObject->setPositionAbsolute(true);
    sensorObject->setPosition(QVector3D(0.0, 0.0, 0.0));
    sensorObject->setTextureImage(color);
    surface->addCustomItem(sensorObject);
    surface->setShadowQuality(QAbstract3DGraph::ShadowQuality::ShadowQualityNone);
}

void MainWindow::initializeCalibrationPlot()
{
    using namespace QtDataVisualization;

    calibScatter = new Q3DScatter();
    QWidget *container = QWidget::createWindowContainer(calibScatter);
    if (!calibScatter->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
    }
    else
    {
        container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        container->setFocusPolicy(Qt::StrongFocus);

        QHBoxLayout *hLayout = new QHBoxLayout(ui->calibrationPlot);
        QVBoxLayout *vLayout = new QVBoxLayout();
        hLayout->addWidget(container, 1);
        hLayout->addLayout(vLayout);
        vLayout->setAlignment(Qt::AlignTop);
    }

    QScatter3DSeries *series = new QScatter3DSeries();
    series->setBaseColor(QColor(0,0,0));
    series->setMeshSmooth(false);
    calibScatter->addSeries(series);

    calibScatter->axisX()->setTitle(QStringLiteral("X"));
    calibScatter->axisY()->setTitle(QStringLiteral("Y"));
    calibScatter->axisZ()->setTitle(QStringLiteral("Z"));

    calibScatter->axisX()->setTitleVisible(true);
    calibScatter->axisY()->setTitleVisible(true);
    calibScatter->axisZ()->setTitleVisible(true);
    calibScatter->setShadowQuality(QAbstract3DGraph::ShadowQuality::ShadowQualityNone);

    calibScatter->setReflection(false);
    calibScatter->setSurfaceType(QSurface::SurfaceType::OpenGLSurface);
}

void MainWindow::setSensorPose(QQuaternion q, QString identifier)
{
    //ui->openGLWidget->setRotation(rm);

    QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(ui->comboSelectPort->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        this->sensorObject->setRotation(q);
        return;
    }
}

void MainWindow::setCalibrationData(QVector3D* point, QString identifier)
{
    if (calibScatter->seriesList().count() == 0)
    {
        qDebug() << "No container found for calibration data";
        return;
    }
    calibScatter->seriesList().at(0)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(*point));
}

void MainWindow::on_btnStartStopCalibration_clicked()
{
    if (ui->btnStartStopCalibration->text() == "Start calibration")
    {
        ui->btnStartStopCalibration->setText("Stop calibration");
        QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPortCalib->itemData(ui->comboSelectPortCalib->currentIndex(), Qt::UserRole)).toString();
        ui->comboSelectPortCalib->setEnabled(false);
        emit beginCalibration(selectedIdentifier);
    }
    else
    {
        ui->btnStartStopCalibration->setText("Start calibration");
        QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPortCalib->itemData(ui->comboSelectPortCalib->currentIndex(), Qt::UserRole)).toString();
        ui->comboSelectPortCalib->setEnabled(true);
        emit stopCalibration(selectedIdentifier);
    }
}

void MainWindow::on_btnSaveCalibration_clicked()
{
    emit saveCalibration("");
}

void MainWindow::on_btnLoadCalibration_clicked()
{
    emit loadCalibration("");
}

void MainWindow::setConfigurationTable(QList<Sensor*> sensors)
{
    QStringList horizontalHeaderLabels = {"Port", "Identifier", "Name", "Baudrate", "Status"};

    ui->tableCurrentConfig->clear();
    ui->tableCurrentConfig->setRowCount(sensors.size());
    ui->tableCurrentConfig->setColumnCount(horizontalHeaderLabels.size());
    ui->tableCurrentConfig->setShowGrid(true);
    ui->tableCurrentConfig->setHorizontalHeaderLabels(horizontalHeaderLabels);

    int row = 0;
    ui->comboSelectPort->clear();
    ui->comboSelectPortCalib->clear();

    foreach (Sensor *sensor, sensors)
    {
        if (sensor == nullptr)
            continue;

        QList<QString> columns;

        columns.append(sensor->portname());
        columns.append(sensor->identifier());
        columns.append(sensor->name());
        columns.append(QString::number(sensor->baudrate()));
        columns.append(QString(QVariant::fromValue(sensor->currentStatus()).toString()));

        ui->comboSelectPort->addItem(sensor->name());
        ui->comboSelectPort->setItemData(ui->comboSelectPort->count()-1, sensor->identifier(), Qt::UserRole);

        ui->comboSelectPortCalib->addItem(sensor->name());
        ui->comboSelectPortCalib->setItemData(ui->comboSelectPortCalib->count()-1, sensor->identifier(), Qt::UserRole);

        for (int col = 0; col < horizontalHeaderLabels.size(); col++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, columns.at(col));
            if (col == 0 || col == 4)
            {
               item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            }
            ui->tableCurrentConfig->setItem(row, col, item);
        }
        row++;
    }

    ui->tableCurrentConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    ui->tableCurrentConfig->resizeColumnsToContents();
    ui->tabWidget->setCurrentIndex(0);
    on_comboSelectPort_currentIndexChanged(ui->comboSelectPort->currentIndex());
}

void MainWindow::setSensorStatus(Sensor::SensorStatus status, QString identifier)
{
    int identifierColumn = whatColumnNumber("Identifier");
    int statusColumn = whatColumnNumber("Status");

    if (statusColumn != -1 && identifierColumn != -1)
    {
        for (int row = 0; row < ui->tableCurrentConfig->rowCount(); row++)
        {
            if (ui->tableCurrentConfig->item(row, identifierColumn)->text() == identifier)
            {
                ui->tableCurrentConfig->item(row, statusColumn)->setText(QVariant::fromValue(status).toString());
                on_comboSelectPort_currentIndexChanged(ui->comboSelectPort->currentIndex());
            }
        }
    }
}

void MainWindow::setSensorData(qint16 *databuf, QString identifier)
{
    QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(ui->comboSelectPort->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        for (int i = 0; i < lineEditList->size(); i++)
        {
            lineEditList->at(i)->setText(QString::number(databuf[i]));
        }
    }
}

void MainWindow::setServiceData(Sensor::ServiceData data, QString identifier)
{
    QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(ui->comboSelectPort->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        ui->lineEditAverageLocalTime->setText(QString::number(1e12/((double)(data.LocalTimeElapsed))));
        ui->lineEditAverageRemoteTime->setText(QString::number(1e9/((double)(data.RemoteTimeElapsed))));
        ui->lineEditMissedPackets->setText(QString::number(data.DeclinedPackets));

    }
}

void MainWindow::on_btnStartStopSwitch_clicked()
{
    if (ui->btnStartStopSwitch->text() == "Start")
    {
        ui->btnLoadConfig->setEnabled(false);
        ui->btnSaveConfig->setEnabled(false);
        ui->btnLoadCalibration->setEnabled(false);
        ui->btnSaveCalibration->setEnabled(false);

        ui->btnStartStopCalibration->setEnabled(true);

        ui->btnStartStopSwitch->setText("Stop");
        emit beginSerial();
    }
    else
    {
        ui->btnLoadConfig->setEnabled(true);
        ui->btnSaveConfig->setEnabled(true);
        ui->btnLoadCalibration->setEnabled(true);
        ui->btnSaveCalibration->setEnabled(true);

        ui->btnStartStopCalibration->setEnabled(false);

        ui->btnStartStopSwitch->setText("Start");
        emit stopSerial();
    }
}

void MainWindow::on_btnLoadConfig_clicked()
{
    emit stopSerial();
    QString path = QFileDialog::getOpenFileName(this, "Open Configuration", "", "XML files (*.xml)");
    emit loadConfig(path);
}

void MainWindow::on_btnSaveConfig_clicked()
{
    QString path = QFileDialog::getSaveFileName(nullptr, "configuration", ".", "XML files (*.xml)" );
    if (!path.isNull() && !path.isEmpty())
    {
        QList<Sensor*> config = formatConfig(ui->tableCurrentConfig);
        emit configurationChangedByUser(config);
        emit saveConfig(path);
    }
}

QList<Sensor *> MainWindow::formatConfig(QTableWidget* table)
{
    QList<Sensor*> sensors;

    int identifierColumn = whatColumnNumber("Identifier");
    int baudrateColumn = whatColumnNumber("Baudrate");
    int nameColumn = whatColumnNumber("Name");
    int portColumn = whatColumnNumber("Port");

    if (identifierColumn != -1 && baudrateColumn !=-1 && nameColumn != -1)
    {
        for (int row = 0; row < ui->tableCurrentConfig->rowCount(); row++)
        {

            if (table->item(row, identifierColumn)->text().size() == 20 &&
                    table->item(row, baudrateColumn)->text().toLong() > 0 &&
                    table->item(row, nameColumn)->text().size() > 0)
            {
                if (table->item(row, portColumn) == nullptr)
                {
                    table->setItem(row, 0, new QTableWidgetItem());
                }
                Sensor *sensor = new Sensor(table->item(row, portColumn)->text(),
                                            table->item(row, identifierColumn)->text(),
                                            table->item(row, baudrateColumn)->text().toLong(),
                                            table->item(row, nameColumn)->text());
                sensors.append(sensor);
            }
            else
            {
                qDebug() << "Skipped sensor at position" << row+1 << ". Bad parameters";
            }
        }
    }
    return sensors;
}

void MainWindow::on_btnAddDevice_clicked()
{
    ui->tableCurrentConfig->insertRow(ui->tableCurrentConfig->rowCount());
}

void MainWindow::on_btnRemoveDevice_clicked()
{
    QModelIndexList indexes = ui->tableCurrentConfig->selectionModel()->selectedRows();
    int countRow = indexes.count();

    for( int i = countRow; i > 0; i--)
    {
        ui->tableCurrentConfig->removeRow(indexes.at(i-1).row());
    }
}

void MainWindow::on_comboSelectPort_currentIndexChanged(int index)
{

    int identifierColumn = whatColumnNumber("Identifier");
    int statusColumn = whatColumnNumber("Status");

    if (statusColumn != -1 && identifierColumn != -1)
    {
        QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(index, Qt::UserRole)).toString();
        if (!selectedIdentifier.isEmpty() && !selectedIdentifier.isNull())
        {
            for (int row = 0; row < ui->tableCurrentConfig->rowCount(); row++)
            {
                if (ui->tableCurrentConfig->item(row, identifierColumn)->text() == selectedIdentifier)
                {
                    ui->lineEditSensorStatus->setText(ui->tableCurrentConfig->item(row, statusColumn)->text());
                }
            }
        }
    }
}

int MainWindow::whatColumnNumber(QString name)
{
    for (int col= 0; col < ui->tableCurrentConfig->columnCount(); col++)
    {
        if (ui->tableCurrentConfig->horizontalHeaderItem(col)->text() == name)
        {
            return col;
        }
    }
    return -1;
}

