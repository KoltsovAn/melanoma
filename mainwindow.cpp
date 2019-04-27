#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = std::make_unique<database>();
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

}

void MainWindow::calculateProgress()
{
    int val = progress->value();
    progress->setValue(++val);
}

void MainWindow::calculateFinish()
{
    trainSampleThread->join();
    std::vector<objectDescription> objects = calculator->getObjects();
    if (item == "Доброкачественная")
        db->insertTrainSample(objects, 0);
    else
        db->insertTrainSample(objects, 1);
    trainSampleThread.reset();
    progress.reset();
}
