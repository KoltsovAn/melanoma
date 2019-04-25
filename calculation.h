#ifndef CALCULATION_H
#define CALCULATION_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDir>
#include <QColor>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <limits>
#include <utility>
#include <mutex>
#include <thread>
#include <pthread.h>

#include "objectdescription.h"

class calculation : public QObject
{
    Q_OBJECT
    std::mutex mutInsertObject;
    QStringList path;
    std::vector<objectDescription> objects;
public:
    calculation(QStringList &filePath, QObject *parent = nullptr);
    ~calculation();

    void startCalculation();
    std::vector<objectDescription> &&getObjects();

signals:
    void progres();
    void finish();
private:
    void atThread(int from, int to); //костыль для запуска потоков
    void calculator(const QString &path); //выполняет всю работу с изображением
    int definitionTresholdBinarization(std::shared_ptr<QImage> img); //автоматическое определение порога бинаризации
    void binarization(std::shared_ptr<QImage> img, int treshold); //бинаризация
    objectDescription &&calculateAttribute(std::shared_ptr<QImage> img); //расчет признаков
    bool region(std::shared_ptr<QImage> file, int x, int y); //проверка является ли пиксель граничным
};

#endif // CALCULATION_H
