#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = std::make_shared<database>();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    bool ok;
    QString item = QInputDialog::getItem(this, "Выбрать нужное",
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
    QDir d(dir);
    d.setFilter(QDir::Files);
    QStringList fileList=d.entryList(QStringList() << "*.bmp" << "*.jpg",
                                     QDir::Files | QDir::NoSymLinks, QDir::Name);
    for(auto &fileName: fileList)
        fileName = dir + "/" + fileName;
    calculation calculator(fileList);
    calculator.startCalculation();
    std::vector<objectDescription> objects = calculator.getObjects();
    if (item == "Доброкачественная")
        db->insertTrainSample(objects, 0);
    else
        db->insertTrainSample(objects, 1);
}

void MainWindow::on_pushButton_2_clicked()
{

}
