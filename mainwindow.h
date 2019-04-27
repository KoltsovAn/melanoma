#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDir>

#include <memory>
#include <thread>

#include "database.h"
#include "calculation.h"

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

    void calculateProgress();
    void calculateFinish();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<database> db;

    std::unique_ptr<calculation> calculator;
    std::unique_ptr<std::thread> trainSampleThread;
    std::shared_ptr<QProgressDialog> progress;
    QString item;
};

#endif // MAINWINDOW_H
