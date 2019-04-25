#include "calculation.h"

#include <QDebug>

const QColor notObjectPixel = Qt::white;

calculation::calculation(QStringList &filePath, QObject *parent) : QObject (parent), path(filePath)
{

}

calculation::~calculation()
{

}

void calculation::startCalculation()
{
    int maxThread = std::thread::hardware_concurrency() - 1;
    if (maxThread < 2)
        for (auto &file: path)
            calculator(file);
    else {
        std::vector<std::thread> threads;
        int step = (path.size() / maxThread) + 1;
        for (int i = 0; i < maxThread; i++)
            threads.push_back(std::thread(&calculation::atThread, this, i * step, step * (i + 1)));
        for (size_t i = 0; i < threads.size(); i++)
            threads.at(i).join();
            //emit progres();
    }
    emit finish();
}

std::vector<objectDescription> &&calculation::getObjects()
{
    return std::move(objects);
}

void calculation::atThread(int from, int to)
{
    for (int i = from; i < to && i < path.size(); i++)
        calculator(path.at(i));
}

void calculation::calculator(const QString &path)
{
    std::shared_ptr<QImage> img = std::make_shared<QImage>(path);
    binarization(img, definitionTresholdBinarization(img));
    objectDescription object = calculateAttribute(img);
    std::lock_guard<std::mutex> lock(mutInsertObject);
    objects.push_back(std::move(object));
}

int calculation::definitionTresholdBinarization(std::shared_ptr<QImage> img)
{
    std::vector<int> temp(256, 0);

    for(int x = 0; x < img->width(); x++)
        for(int y = 0; y < img->height(); y++)
        {
            QRgb rgb = img->pixel(x, y);
            temp.at((qGreen(rgb) + qRed(rgb) + qBlue(rgb)) / 3)++;// заполнение гистограммы
        }

    for(int i = 0; i < 254; i++)
    {
        temp.at(i) =temp.at(i + 1) - temp.at(i);//из следующего вычитаем текущее
    }
    std::vector<int> max;
    for(int i = 60; i < 244; i++)
    {
        int count = 0;
        for(int j = 0; j < 10; j++)//проверка в окне равным 10
            if(temp.at(i + j) > 0)
                count++;
        if(temp.at(i) > 0 && count > 5)
        {
            max.push_back(i);
            i += 10;
        }
    }
    int k = 0;
    for(int i: max)
        k += i;
    k /= max.size(); // определение среднего значения локальных максимумов
    return k;
}

void calculation::binarization(std::shared_ptr<QImage> img, int treshold)
{
    for (int x = 0; x < img->width(); x++) {
        for (int y = 0; y < img->height(); y++) {
            QRgb rgb = img->pixel(x, y);
            if (treshold < ((qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3))
                img->setPixelColor(x, y, Qt::white);
        }
    }
}

objectDescription &&calculation::calculateAttribute(std::shared_ptr<QImage> img)
{
    objectDescription object;
    std::vector< std::vector <double> > GR(256, std::vector <double>(256,0.0));
    std::vector< std::vector <double> > GG(256, std::vector <double>(256,0.0));
    std::vector< std::vector <double> > GB(256, std::vector <double>(256,0.0));
    std::vector< std::vector <double> > G(256, std::vector <double>(256,0.0));
    int n = 0;
    QPoint centerMass(0, 0);
    std::vector<QPoint> border;
    QColor curentPixel, nextPixel;
    for (int x = 0; x < img->width() - 1; x++) {
        for (int y = 0; y < img->height(); y++) {
            curentPixel = img->pixelColor(x, y);
            n++;
            object.H += curentPixel.hslHue();
            object.Sat += curentPixel.hslSaturation();
            if (curentPixel != notObjectPixel) {
                nextPixel = img->pixelColor(x + 1, y);
                centerMass.rx() += x;
                centerMass.ry() += y;
                if (region(img, x, y)) {
                    border.push_back(QPoint(x, y));
                }
                if (nextPixel != notObjectPixel) {
//                    qDebug() << curentPixel.red() << "   " << nextPixel.red();
//                    qDebug() << curentPixel.green() << "   " << nextPixel.green();
//                    qDebug() << curentPixel.blue() << "   " << nextPixel.blue();
                    GR.at(curentPixel.red()).at(nextPixel.red())++;
                    GG.at(curentPixel.green()).at(nextPixel.green())++;
                    GB.at(curentPixel.blue()).at(nextPixel.blue())++;
                    G.at((curentPixel.red() + curentPixel.green() + curentPixel.blue()) / 3).
                            at((nextPixel.red() + nextPixel.green() + nextPixel.blue()) / 3)++;
                }
            }
        }
    }
    object.K = std::pow(border.size(), 2) / n;
    object.S = n;
    object.H /= n;
    object.Sat /= n;
    centerMass.rx() /= n;
    centerMass.ry() /= n;
    for(size_t i = 0; i < 256; i++)
        for(size_t j = 0; j < 256; j++)
        {
            GR[i][j] /= n;
            GG[i][j] /= n;
            GB[i][j] /= n;
            G[i][j] /= n;
            object.ASM_R += pow(GR[i][j], 2);
            object.ASM_G += pow(GG[i][j], 2);
            object.ASM_B += pow(GB[i][j], 2);
            object.ASM_L += pow(G[i][j], 2);
            object.CON_R += (pow(i - j,2)*GR[i][j]);
            object.CON_G += (pow(i - j,2)*GG[i][j]);
            object.CON_B += (pow(i - j,2)*GB[i][j]);
            object.CON_L += (pow(i - j,2)*G[i][j]);
            if(GR[i][j] != 0.0)
                object.ENT_R -= (GR[i][j] * std::log10(GR[i][j]));
            if(GG[i][j] != 0.0)
                object.ENT_G -= (GG[i][j] * std::log10(GG[i][j]));
            if(GB[i][j] != 0.0)
                object.ENT_B -= (GB[i][j] * std::log10(GB[i][j]));
            if(G[i][j] != 0.0)
                object.ENT_L -= (GB[i][j] * std::log10(G[i][j]));
            object.LUN_R += (GR[i][j] / (1 + pow(i - j, 2)));
            object.LUN_G += (GG[i][j] / (1 + pow(i - j, 2)));
            object.LUN_B += (GB[i][j] / (1 + pow(i - j, 2)));
            object.LUN_L += (G[i][j] / (1 + pow(i - j, 2)));
            object.MPR_R = std::max(object.MPR_R, GR[i][j]);
            object.MPR_G = std::max(object.MPR_G, GG[i][j]);
            object.MPR_B = std::max(object.MPR_B, GB[i][j]);
            object.MPR_L = std::max(object.MPR_L, G[i][j]);
        }
    double iMax = std::numeric_limits<double>::max(), iMin = std::numeric_limits<double>::min();
    for (size_t i = 0; i < border.size(); i++) {
        double length = std::sqrt(std::pow((centerMass.rx() - border.at(i).rx()),2) -
                                  std::pow((centerMass.ry() - border.at(i).ry()),2));
        iMax = std::max(length, iMax);
        iMin = std::min(length, iMin);
    }
    object.IMM = iMin / iMax;
    object.D = iMax + iMin;
    return std::move(object);
}

bool calculation::region(std::shared_ptr<QImage> file, int x, int y)
{
    
    if (x - 1 < 0 || x + 1 >= file->width() || y - 1 < 0 || y + 1 >= file->height())
        return true;
    QColor ne = file->pixelColor(x, y + 1);
    if(ne == notObjectPixel)
        return true;
    ne=file->pixelColor(x - 1, y);
    if(ne == notObjectPixel)
        return true;
    ne=file->pixelColor(x + 1, y);
    if(ne == notObjectPixel)
        return true;
    ne=file->pixelColor(x, y - 1);
    if(ne == notObjectPixel)
        return true;
    return false;
}
