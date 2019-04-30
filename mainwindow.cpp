#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = std::make_unique<database>();

    scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (trainSampleThread)
        return;
    bool ok;
    /*QString */item = QInputDialog::getItem(this, "Выбрать нужное",
                                         "Тип меланомы:", QStringList() << "Доброкачественная" << "Злокачественная",
                                         0, false, &ok);
    if (!ok && !item.isEmpty())
        return;
    QString dir = QFileDialog::getExistingDirectory(this, "Выбрать папку с " + item,
                                                    "D:/QtProject/Koltsov",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    QStringList fileList = QDir(dir).entryList(QStringList() << "*.bmp" << "*.jpg" << "*.png",
                                               QDir::Files | QDir::NoSymLinks, QDir::Name);
    progress = std::make_shared<QProgressDialog>("Расчет характеристик...", nullptr, 0,
                                                 fileList.size() + 1, this);
    progress->setWindowTitle("   ");
    progress->setWindowModality(Qt::WindowModal);
    for(auto &fileName: fileList)
        fileName = dir + "/" + fileName;
    calculator = std::make_unique<calculation>(fileList);
    connect(calculator.get(), SIGNAL(progres()), this, SLOT(calculateProgress()));
    connect(calculator.get(), SIGNAL(finish()), this, SLOT(calculateFinish()));
    trainSampleThread = std::make_unique<std::thread>(std::thread([&]{calculator->startCalculation();}));
}

void MainWindow::on_pushButton_2_clicked()
{
    if (trainSampleThread)
        return;
    classificator = std::make_unique<baesClassificator>();
    progress = std::make_shared<QProgressDialog>("Обучение классификатора...", nullptr, 0,
                                                 classificator->getTrainCount(), this);
    progress->setWindowTitle("   ");
    progress->setWindowModality(Qt::WindowModal);
    connect(classificator.get(), SIGNAL(progres()), this, SLOT(calculateProgress()));
    connect(classificator.get(), SIGNAL(finish()), this, SLOT(trainFinish()));
    trainSampleThread = std::make_unique<std::thread>(std::thread([&]{classificator->startTrain();}));
}

void MainWindow::on_pushButton_3_clicked()
{
    if (trainSampleThread)
        return;
    QString dir = QFileDialog::getExistingDirectory(this, "Выбрать папку с " + item,
                                                    "D:/QtProject/Koltsov",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;
    progress = std::make_shared<QProgressDialog>("Тестирование классификатора...", nullptr, 0,
                                                 10, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setWindowTitle("   ");
    progress->setValue(0);
    QStringList fileList = QDir(dir).entryList(QStringList() << "*.bmp" << "*.jpg" << "*.png",
                                               QDir::Files | QDir::NoSymLinks, QDir::Name);
    progress->setMaximum(fileList.size() + 1);
    for(auto &fileName: fileList)
        fileName = dir + "/" + fileName;
    classificator = std::make_unique<baesClassificator>(fileList);
    connect(classificator.get(), SIGNAL(progres()), this, SLOT(calculateProgress()));
    connect(classificator.get(), SIGNAL(finish()), this, SLOT(testFinish()));
    trainSampleThread = std::make_unique<std::thread>(std::thread([&]{classificator->startTest();}));
}

void MainWindow::calculateProgress()
{
    progress->setValue(progress->value() + 1);
}

void MainWindow::calculateFinish()
{
    trainSampleThread->join();
    std::vector<objectDescription> objects = calculator->getObjects();
    if (item == "Доброкачественная")
        db->insertTrainSample(objects, 0);
    else
        db->insertTrainSample(objects, 1);
    QMessageBox msgBox;
    msgBox.setText("Заполнение тестовой выборки завершено.");
    msgBox.exec();
    trainSampleThread.reset();
    progress.reset();
}

void MainWindow::trainFinish()
{
    trainSampleThread->join();
    QMessageBox msgBox;
    msgBox.setText("Обучение завершено.");
    msgBox.exec();
    trainSampleThread.reset();
    classificator.reset();
    progress.reset();
}

void MainWindow::testFinish()
{
    scene->clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setColumnCount(0);
    ui->tableWidget_2->clear();
    ui->tableWidget_2->setRowCount(1);
    ui->tableWidget_2->setColumnCount(0);
    trainSampleThread->join();
    std::vector<objectDescription> objects = classificator->getObjects();
    QTableWidgetItem *ti;
    for (auto &obj: objects){
        ti = new QTableWidgetItem();
        ti->setData(Qt::DecorationRole, QPixmap(obj.fileName)/*.scaled(100, 100)*/);
        switch (obj.type) {
        case 0:
            ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() + 1);
            ui->tableWidget->setItem(0,ui->tableWidget->columnCount() - 1, ti);
            break;
        case 1:
            ui->tableWidget_2->setColumnCount(ui->tableWidget_2->columnCount() + 1);
            ui->tableWidget_2->setItem(0, ui->tableWidget_2->columnCount() - 1, ti);
            break;
        }
    }
    QMessageBox msgBox;
    msgBox.setText("Тестирование завершено.\n"
                   "Изначально было: " + QString::number(objects.size()) +".\n"
                   "К доброкачественной отнесено : " + QString::number(ui->tableWidget->columnCount()) +".\n"
                   "К злокачественной отнесено : " + QString::number(ui->tableWidget_2->columnCount()) +".\n");
    msgBox.exec();
    trainSampleThread.reset();
    classificator.reset();
    progress.reset();
}
