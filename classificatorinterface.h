#ifndef CLASSIFICATORINTERFACE_H
#define CLASSIFICATORINTERFACE_H

#include <QObject>

#include <memory>
#include <utility>

#include "calculation.h"
#include "database.h"

class classificatorInterface : public QObject
{
    Q_OBJECT

protected:
    std::unique_ptr<database> db;
    QStringList filePath;
    std::vector<objectDescription> objects;
public:
    classificatorInterface(QStringList &path, QObject *parent = nullptr) : QObject(parent), filePath(path) {
        db = std::make_unique<database>();
    }
    virtual ~classificatorInterface() {}

    virtual void startTrain() {}
    virtual void startTest() {}

    std::vector<objectDescription> getObjects() {
        return std::move(objects);
    }
signals:
    void progres();
    void finish();

};

#endif // CLASSIFICATORINTERFACE_H
