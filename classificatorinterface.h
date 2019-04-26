#ifndef CLASSIFICATORINTERFACE_H
#define CLASSIFICATORINTERFACE_H

#include <QObject>

#include "calculation.h"
#include "database.h"

class classificatorInterface : public QObject
{
    Q_OBJECT
public:
    classificatorInterface(QObject *parent = nullptr);
    ~classificatorInterface();

    void startTrain();
    void startTest();
signals:
    void progres();
    void finish();

};

#endif // CLASSIFICATORINTERFACE_H
