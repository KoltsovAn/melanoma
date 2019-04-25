#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QFile>
#include <vector>
#include <QString>

#include "objectdescription.h"

#define DATABASE_HOSTNAME           "Classifier"
#define DATABASE_NAME               "Classifier.db"

#define TABLE_TRAINSAMPLE           "TrainSample"

//const std::list<QString> typeMarkerName = {"K", "S", "Sat", "H", "IMM", "D",
//                                           "ASM_R", "CON_R", "ENT_R", "LUN_R", "MPR_R",
//                                           "ASM_G", "CON_G", "ENT_G", "LUN_G", "MPR_G",
//                                           "ASM_B", "CON_B", "ENT_B", "LUN_B", "MPR_B",
//                                           "ASM_L", "CON_L", "ENT_L", "LUN_L", "MPR_L"};

class database : public QObject
{
public:
    explicit database(QObject *parent = nullptr);
    ~database();


    void connectToDataBase();
    void insertTrainSample(std::vector<objectDescription> &objects, int type);

private:
    // Сам объект базы данных, с которым будет производиться работа
    QSqlDatabase db;

    // Внутренние методы для работы с базой данных
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createTable();
};

#endif // DATABASE_H
