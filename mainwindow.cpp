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

    /******************************************************************************************************************************************************/

    using namespace QtDataVisualization;

    Q3DSurface *graph = new Q3DSurface();
    QWidget *dcontainer = QWidget::createWindowContainer(graph);
    //! [0]

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
    }
    else
    {
        dcontainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        dcontainer->setFocusPolicy(Qt::StrongFocus);

        QHBoxLayout *hLayout = new QHBoxLayout(ui->drawingPlot);
        QVBoxLayout *vLayout = new QVBoxLayout();
        hLayout->addWidget(dcontainer, 1);
        hLayout->addLayout(vLayout);
        vLayout->setAlignment(Qt::AlignTop);
    }

    graph->axisX()->setRange(-3.0f, 3.0f);
    graph->axisY()->setRange(-3.0f, 3.0f);
    graph->axisZ()->setRange(-3.0f, 3.0f);

    graph->axisX()->setTitle(QStringLiteral("X"));
    graph->axisY()->setTitle(QStringLiteral("Y"));
    graph->axisZ()->setTitle(QStringLiteral("Z"));

    graph->axisX()->setTitleVisible(true);
    graph->axisY()->setTitleVisible(true);
    graph->axisZ()->setTitleVisible(true);

    QImage color = QImage(2, 2, QImage::Format_RGB32);
    color.fill(Qt::red);

    this->lsm9ds1obj = new QCustom3DItem();
    lsm9ds1obj->setMeshFile("E:/QtProjects/Serializer/solids/brick.obj");

    lsm9ds1obj->setScalingAbsolute(true);
    lsm9ds1obj->setScaling(QVector3D(2, 2, 2));
    lsm9ds1obj->setPositionAbsolute(true);
    lsm9ds1obj->setPosition(QVector3D(0.0, 0.0, 0.0));

    lsm9ds1obj->setTextureImage(color);

    graph->addCustomItem(lsm9ds1obj);

    lsm9ds1obj->setObjectName("LSM9DS1");

    /******************************************************************************************************************************************************/
    Q3DScatter *scatter = new Q3DScatter();
    QWidget *ccontainer = QWidget::createWindowContainer(scatter);
    if (!scatter->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
    }
    else
    {
        ccontainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ccontainer->setFocusPolicy(Qt::StrongFocus);

        QHBoxLayout *hLayout = new QHBoxLayout(ui->calibrationPlot);
        QVBoxLayout *vLayout = new QVBoxLayout();
        hLayout->addWidget(ccontainer, 1);
        hLayout->addLayout(vLayout);
        vLayout->setAlignment(Qt::AlignTop);
    }

    QScatterDataArray *data = new QScatterDataArray;
    *data << QVector3D(0.5f, 0.5f, 0.5f) << QVector3D(-0.3f, -0.5f, -0.4f) << QVector3D(0.0f, -0.3f, 0.2f);
    scatter->addSeries(new QScatter3DSeries);
    scatter->seriesList().at(0)->dataProxy()->resetArray(data);
    scatter->show();
}

void MainWindow::setSensorPose(QQuaternion q, QString identifier)
{
    //ui->openGLWidget->setRotation(rm);
    QString selectedIdentifier = QVariant::fromValue(ui->comboSelectPort->itemData(ui->comboSelectPort->currentIndex(), Qt::UserRole)).toString();
    if (identifier == selectedIdentifier)
    {
        this->lsm9ds1obj->setRotation(q);
        return;
    }
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
        ui->btnStartStopSwitch->setText("Stop");
        emit beginSerial();
    }
    else
    {
        ui->btnLoadConfig->setEnabled(true);
        ui->btnSaveConfig->setEnabled(true);
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
