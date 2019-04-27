#ifndef BAECCLASSIFICATOR_H
#define BAECCLASSIFICATOR_H

#include "classificatorinterface.h"


class baesClassificator : public classificatorInterface
{
public:
    baesClassificator(QStringList &path);
    virtual ~baesClassificator() final;

    virtual void startTrain() final;
    virtual void startTest() final;
};

#endif // BAECCLASSIFICATOR_H
