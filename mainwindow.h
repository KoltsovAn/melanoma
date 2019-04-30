#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDir>
#include <QGraphicsScene>

#include <memory>
#include <thread>

#include "database.h"
#include "calculation.h"
#include "baesclassificator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void calculateProgress();
    void calculateFinish();

    void trainFinish();

    void testFinish();


private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    std::unique_ptr<database> db;

    std::unique_ptr<calculation> calculator;
    std::unique_ptr<std::thread> trainSampleThread;
    std::shared_ptr<QProgressDialog> progress;
    QString item;

//    std::vector<std::unique_ptr<classificatorInterface> > classificators;
    std::unique_ptr<classificatorInterface> classificator;
};

#endif // MAINWINDOW_H
