#ifndef BAECCLASSIFICATOR_H
#define BAECCLASSIFICATOR_H

#include "classificatorinterface.h"


class baesClassificator : public classificatorInterface
{
public:
    baesClassificator();
    baesClassificator(QStringList &path);
    virtual ~baesClassificator() final;

    virtual void startTrain() final;
    virtual void startTest() final;
    virtual size_t getTrainCount() final;
};

#endif // BAECCLASSIFICATOR_H
