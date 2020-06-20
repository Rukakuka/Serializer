#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "geometryengine.h"

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::Mainwindow)
{
    qRegisterMetaType<MainWindow::CalibrationMode>();

    ui->setupUi(this);

    this->leRawData = new QList<QLineEdit*>;

    leRawData->append(ui->lineEditAx);
    leRawData->append(ui->lineEditAy),
    leRawData->append(ui->lineEditAz),
    leRawData->append(ui->lineEditGx),
    leRawData->append(ui->lineEditGy),
    leRawData->append(ui->lineEditGz),
    leRawData->append(ui->lineEditMx),
    leRawData->append(ui->lineEditMy),
    leRawData->append(ui->lineEditMz);

    ui->tCurrentConfig->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->tCurrentConfig->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    ui->btnStartStopSwitch->setText("Start");
    ui->btnStartStopCalibration->setText("Start calibration");
    ui->btnStartStopCalibration->setEnabled(false);

    initializeDrawingPlot();
    initializeCalibrationPlot();
    ui->leCalibCtn->setText("0");
    ui->comboSelectCalibMode->addItem("New calibration");
    ui->comboSelectCalibMode->addItem("Load from file");
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

    this->sensorVisu = new QCustom3DItem();
    try
    {
        sensorVisu->setMeshFile("E:/QtProjects/Serializer/solids/brick.obj");
    }
    catch (...)
    {
       qDebug() << "Error opening mesh";
    }
    sensorVisu->setScalingAbsolute(true);
    sensorVisu->setScaling(QVector3D(2, 2, 2));
    sensorVisu->setPositionAbsolute(true);
    sensorVisu->setPosition(QVector3D(0.0, 0.0, 0.0));
    sensorVisu->setTextureImage(color);

    surface->addCustomItem(sensorVisu);
    surface->setShadowQuality(QAbstract3DGraph::ShadowQuality::ShadowQualityNone);
}

void MainWindow::initializeCalibrationPlot()
{
    using namespace QtDataVisualization;

    scatter = new Q3DScatter();
    QWidget *container = QWidget::createWindowContainer(scatter);

    if (!scatter->hasContext()) {
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

    QScatter3DSeries *series1 = new QScatter3DSeries();
    series1->setBaseColor(Qt::red);
    series1->setMeshSmooth(false);
    series1->setMesh(QSurface3DSeries::MeshPoint);
    scatter->addSeries(series1);

    QScatter3DSeries *series2 = new QScatter3DSeries();
    series2->setBaseColor(Qt::blue);
    series2->setMeshSmooth(false);
    series2->setMesh(QSurface3DSeries::MeshPoint);
    scatter->addSeries(series2);

    scatter->axisX()->setTitle(QStringLiteral("X"));
    scatter->axisY()->setTitle(QStringLiteral("Y"));
    scatter->axisZ()->setTitle(QStringLiteral("Z"));

    scatter->axisX()->setTitleVisible(true);
    scatter->axisY()->setTitleVisible(true);
    scatter->axisZ()->setTitleVisible(true);
    scatter->setShadowQuality(QAbstract3DGraph::ShadowQuality::ShadowQualityNone);

    scatter->setReflection(false);
    scatter->setSurfaceType(QSurface::SurfaceType::OpenGLSurface);
}

void MainWindow::setSensorPose(QQuaternion q, QString identifier)
{
    //ui->openGLWidget->setRotation(rm);

    QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(ui->cSelectPortView->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        this->sensorVisu->setRotation(q);
        return;
    }
}

void MainWindow::addPointToScatter(QVector3D* point, QString identifier)
{
    if (scatter->seriesList().count() == 0)
    {
        qDebug() << "No container found for scatter point";
        return;
    }
    if (point == nullptr)
        return;
    QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(ui->cSelectPortView->currentIndex(), Qt::UserRole)).toString();
    if (identifier != selectedIdentifier)
    {
        qDebug() << "Wrong sensor id when add point to scatter";
        return;
    }
    scatter->seriesList().at(0)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(*point));
    uint counter = ui->leCalibCtn->text().toUInt();
    ui->leCalibCtn->setText(QString::number(++counter));
}

void MainWindow::setCalibrationData(SensorGeometry::CalibrationData *data, QString identifier)
{
    using namespace QtDataVisualization;
    QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(ui->cSelectPortView->currentIndex(), Qt::UserRole)).toString();
    if (scatter->seriesList().count() < 2)
    {
        qDebug() << "No container found for scatter calibrated data";
        return;
    }
    if (selectedIdentifier != identifier)
    {
        qDebug() << "Wrong sensor id when set calibration data";
        return;
    }
    QProgressBar bar;
    bar.setRange(0, 100);
    bar.setValue(0);
    bar.show();
    for (int i = 0; i < data->calibratedData->count(); i++)
    {
        bar.setValue(i/data->calibratedData->count());
        scatter->seriesList().at(1)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(*(data->calibratedData->at(i))));
    }
    bar.close();

    scatter->axisX()->setRange(-1000.0f, 1000.0f);
    scatter->axisY()->setRange(-1000.0f, 1000.0f);
    scatter->axisZ()->setRange(-1000.0f, 1000.0f);

    // calibScatter->seriesList().at(0)->dataProxy()->removeItems(0, calibScatter->seriesList().at(0)->dataProxy()->itemCount());

    ui->leM11->setText(QString::number(data->matrix(0,0)));
    ui->leM21->setText(QString::number(data->matrix(1,0)));
    ui->leM31->setText(QString::number(data->matrix(2,0)));
    ui->leM12->setText(QString::number(data->matrix(0,1)));
    ui->leM22->setText(QString::number(data->matrix(1,1)));
    ui->leM32->setText(QString::number(data->matrix(2,1)));
    ui->leM13->setText(QString::number(data->matrix(0,2)));
    ui->leM23->setText(QString::number(data->matrix(1,2)));
    ui->leM33->setText(QString::number(data->matrix(2,2)));
    ui->leXbias->setText(QString::number(data->bias.x()));
    ui->leYbias->setText(QString::number(data->bias.y()));
    ui->leZbias->setText(QString::number(data->bias.z()));
}

void MainWindow::on_btnStartStopCalibration_clicked()
{
    if (ui->btnStartStopCalibration->text() == "Start calibration")
    {
        ui->leCalibCtn->setText("0");
        ui->btnStartStopCalibration->setText("Stop calibration");
        QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(ui->cSelectPortView->currentIndex(), Qt::UserRole)).toString();
        ui->cSelectPortView->setEnabled(false);
        if (ui->comboSelectCalibMode->currentText() == "New calibration")
        {
            emit beginCalibration(selectedIdentifier, CalibrationMode::NewCalibration);
        }
        else if (ui->comboSelectCalibMode->currentText() == "Load from file")
        {
            emit beginCalibration(selectedIdentifier, CalibrationMode::LoadFromFile);
        }
    }
    else
    {
        ui->btnStartStopCalibration->setText("Start calibration");
        QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(ui->cSelectPortView->currentIndex(), Qt::UserRole)).toString();
        ui->cSelectPortView->setEnabled(true);
        emit stopCalibration(selectedIdentifier);
    }
}

void MainWindow::on_btnSaveCalibration_clicked()
{
    emit saveCalibration("");
}

void MainWindow::on_btnLoadCalibration_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Open Configuration", "", "XML files (*.xml)");
    if (path.isNull() || path.isEmpty())
        return;
    emit loadCalibration(path);
}

void MainWindow::setConfigurationTable(QList<Sensor*> sensors)
{
    QStringList hhlabels = {"Port", "Identifier", "Name", "Baudrate", "Status"};

    ui->tCurrentConfig->clear();
    ui->tCurrentConfig->setRowCount(sensors.size());
    ui->tCurrentConfig->setColumnCount(hhlabels.size());
    ui->tCurrentConfig->setShowGrid(true);
    ui->tCurrentConfig->setHorizontalHeaderLabels(hhlabels);

    int row = 0;
    ui->cSelectPortView->clear();

    foreach (Sensor *sensor, sensors)
    {
        if (sensor == nullptr)
            continue;

        QList<QString> columns = { sensor->portname(),
                                      sensor->identifier(),
                                      sensor->name(),
                                      QString::number(sensor->baudrate()),
                                      QString(QVariant::fromValue(sensor->currentStatus()).toString())};

        ui->cSelectPortView->addItem(sensor->name());
        ui->cSelectPortView->setItemData(ui->cSelectPortView->count()-1, sensor->identifier(), Qt::UserRole);

        for (int col = 0; col < hhlabels.size(); col++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, columns.at(col));
            if (col == 0 || col == 4)
            {
               item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            }
            ui->tCurrentConfig->setItem(row, col, item);
        }
        row++;
    }

    ui->tCurrentConfig->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Interactive);
    ui->tCurrentConfig->resizeColumnsToContents();
    ui->tabs->setCurrentIndex(0);
    on_cSelectPortView_currentIndexChanged(ui->cSelectPortView->currentIndex());
}


void MainWindow::setSensorStatus(Sensor::SensorStatus status, QString identifier)
{
    int identifierColumn = whatColumnNumber("Identifier");
    int statusColumn = whatColumnNumber("Status");

    if (statusColumn != -1 && identifierColumn != -1)
    {
        for (int row = 0; row < ui->tCurrentConfig->rowCount(); row++)
        {
            if (ui->tCurrentConfig->item(row, identifierColumn)->text() == identifier)
            {
                ui->tCurrentConfig->item(row, statusColumn)->setText(QVariant::fromValue(status).toString());
                on_cSelectPortView_currentIndexChanged(ui->cSelectPortView->currentIndex());
            }
        }
    }
}

void MainWindow::setSensorData(qint16 *databuf, QString identifier)
{
    QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(ui->cSelectPortView->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        for (int i = 0; i < leRawData->size(); i++)
        {
            leRawData->at(i)->setText(QString::number(databuf[i]));
        }
    }
}

void MainWindow::setServiceData(Sensor::ServiceData data, QString identifier)
{
    QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(ui->cSelectPortView->currentIndex(), Qt::UserRole)).toString();
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
        QList<Sensor*> config = formatConfig(ui->tCurrentConfig);
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
        for (int row = 0; row < ui->tCurrentConfig->rowCount(); row++)
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
    ui->tCurrentConfig->insertRow(ui->tCurrentConfig->rowCount());
}

void MainWindow::on_btnRemoveDevice_clicked()
{
    QModelIndexList indexes = ui->tCurrentConfig->selectionModel()->selectedRows();
    int countRow = indexes.count();

    for( int i = countRow; i > 0; i--)
    {
        ui->tCurrentConfig->removeRow(indexes.at(i-1).row());
    }
}

void MainWindow::on_cSelectPortView_currentIndexChanged(int index)
{

    int identifierColumn = whatColumnNumber("Identifier");
    int statusColumn = whatColumnNumber("Status");

    if (statusColumn != -1 && identifierColumn != -1)
    {
        QString selectedIdentifier = QVariant::fromValue(ui->cSelectPortView->itemData(index, Qt::UserRole)).toString();
        if (!selectedIdentifier.isEmpty() && !selectedIdentifier.isNull())
        {
            for (int row = 0; row < ui->tCurrentConfig->rowCount(); row++)
            {
                if (ui->tCurrentConfig->item(row, identifierColumn)->text() == selectedIdentifier)
                {
                    ui->lineEditSensorStatus->setText(ui->tCurrentConfig->item(row, statusColumn)->text());
                }
            }
        }
    }
}

int MainWindow::whatColumnNumber(QString name)
{
    for (int col= 0; col < ui->tCurrentConfig->columnCount(); col++)
    {
        if (ui->tCurrentConfig->horizontalHeaderItem(col)->text() == name)
        {
            return col;
        }
    }
    return -1;
}

