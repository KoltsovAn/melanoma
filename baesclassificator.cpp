#include "baesclassificator.h"
#include <QDebug>

const size_t divider = 50;
const size_t countType = 2;

baesClassificator::baesClassificator() : classificatorInterface ()
{

}

baesClassificator::baesClassificator(QStringList &path) : classificatorInterface (path)
{

}

baesClassificator::~baesClassificator()
{

}

void baesClassificator::startTrain()
{
    db->clearingBaecSetting();
    for (size_t i = 0; i < countType; i++) {
        std::vector<std::vector<double> > data = db->getTrainSample(i);
        for (size_t j = 0; j < db->getCountMark(); j++) {
            double max = std::max_element(data.begin(), data.end(),
                                          [=](const std::vector<double> &a, const std::vector<double> &b) {
                                            return (a.at(j) < b.at(j));})->at(j);

            double min = std::min_element(data.begin(), data.end(),
                                          [=](const std::vector<double> &a, const std::vector<double> &b) {
                                            return (a.at(j) < b.at(j));})->at(j);
            double step = (std::abs(max - min) / divider);
            int n = 1;
            for (double x = min; x < max; x += step) {
                for (auto &cell : data)
                    if (x <= cell.at(j) && cell.at(j) < x + step)
                        ++n;
                db->insertBaesSetting(i, j + 1, x, x + step, 1.0 * n / data.size());
                n = 1;
                emit progres();
            }
        }
    }
    emit finish();
}

void baesClassificator::startTest()
{
    calculation calculate(filePath);
    calculate.startCalculation();
    objects = calculate.getObjects();
    for(auto &obj: objects) {
        double probabilityClass = 0.0;
        size_t idClass = 0;
        for (size_t i = 0; i < countType; i++) {
            double probability = 0.5;
            probability *= db->getProbabilty(i, 1, obj.K);
            probability *= db->getProbabilty(i, 2, obj.S);
            probability *= db->getProbabilty(i, 3, obj.Sat);
            probability *= db->getProbabilty(i, 4, obj.H);
            probability *= db->getProbabilty(i, 5, obj.IMM);
            probability *= db->getProbabilty(i, 6, obj.D);
            probability *= db->getProbabilty(i, 7, obj.ASM_R);
            probability *= db->getProbabilty(i, 8, obj.CON_R);
            probability *= db->getProbabilty(i, 9, obj.ENT_R);
            probability *= db->getProbabilty(i, 10, obj.LUN_R);
            probability *= db->getProbabilty(i, 11, obj.MPR_R);
            probability *= db->getProbabilty(i, 12, obj.ASM_G);
            probability *= db->getProbabilty(i, 13, obj.CON_G);
            probability *= db->getProbabilty(i, 14, obj.ENT_G);
            probability *= db->getProbabilty(i, 15, obj.LUN_G);
            probability *= db->getProbabilty(i, 16, obj.MPR_G);
            probability *= db->getProbabilty(i, 17, obj.ASM_B);
            probability *= db->getProbabilty(i, 18, obj.CON_B);
            probability *= db->getProbabilty(i, 19, obj.ENT_B);
            probability *= db->getProbabilty(i, 20, obj.LUN_B);
            probability *= db->getProbabilty(i, 21, obj.MPR_B);
            probability *= db->getProbabilty(i, 22, obj.ASM_L);
            probability *= db->getProbabilty(i, 23, obj.CON_L);
            probability *= db->getProbabilty(i, 24, obj.ENT_L);
            probability *= db->getProbabilty(i, 25, obj.LUN_L);
            probability *= db->getProbabilty(i, 26, obj.MPR_L);
            if(probabilityClass < probability) {
                probabilityClass = probability;
                idClass = i;
            }
        }
        obj.type = idClass;
        emit progres();
    }
    emit finish();
}

size_t baesClassificator::getTrainCount()
{
    return db->getCountMark() * countType * divider + db->getCountMark();
}
