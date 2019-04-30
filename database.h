#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>
#include <QFile>
#include <QString>

#include <vector>

#include "objectdescription.h"

#define DATABASE_HOSTNAME           "Classifier"
#define DATABASE_NAME               "Classifier.db"

#define TABLE_TRAINSAMPLE           "TrainSample"
#define TABLE_TYPEMARK              "Mark"
#define TABLE_BAESSETTING           "BaesClassificatorSetting"

// 0 - Доброкачественная, 1 - Злокачественная

class database : public QObject
{
public:
    database(QObject *parent = nullptr);
    ~database();

    void insertTrainSample(std::vector<objectDescription> &objects, int type);// заполняем таблицу с обучающей выборкой

    size_t getCountMark();// получаем количество признаков
    bool clearingBaecSetting();// удаление настроек баесовского классификатора
    std::vector<std::vector<double> > getTrainSample(size_t id);// получаем обучающую выборку для определенного типа
    bool insertBaesSetting(size_t idType, size_t idMark, double from, double to, double val);// заполнение таблицы настройки баесовского классификатора
    double getProbabilty(size_t idType, int idMark, double x);

private:
    // Сам объект базы данных, с которым будет производиться работа
    static QSqlDatabase db;

    // заполнение таблиц значениями по умолчанию
    bool initialTableMark();

    // Внутренние методы для работы с базой данных
    void connectToDataBase();
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createTable();
};

#endif // DATABASE_H
