#include "database.h"
#include <QDebug>


const std::list<QString> typeMarkerName = {"K", "S", "Sat", "H", "IMM", "D",
                                           "ASM_R", "CON_R", "ENT_R", "LUN_R", "MPR_R",
                                           "ASM_G", "CON_G", "ENT_G", "LUN_G", "MPR_G",
                                           "ASM_B", "CON_B", "ENT_B", "LUN_B", "MPR_B",
                                           "ASM_L", "CON_L", "ENT_L", "LUN_L", "MPR_L"};

QSqlDatabase database::db = QSqlDatabase::addDatabase("QSQLITE");// ебучая магия
                                                                 // т.к. переменная статик нужно определить ее место в памяти
                                                                 // автоматом это не делается так что руками вот такая херня
// читать: https://stackoverflow.com/questions/9110487/undefined-reference-to-a-static-member

database::database(QObject *parent) : QObject(parent)
{
    this->connectToDataBase();
}

database::~database()
{

}

void database::connectToDataBase()
{
    /* Перед подключением к базе данных производим проверку на её существование.
     * В зависимости от результата производим открытие базы данных или её восстановление
     * */
    if(!QFile(".\\" DATABASE_NAME).exists()){
        this->restoreDataBase();
    } else {
        this->openDataBase();
    }
}

void database::insertTrainSample(std::vector<objectDescription> &objects, int type)
{
    QSqlQuery query;
    for (auto &object: objects)
        if(!query.exec("INSERT INTO " TABLE_TRAINSAMPLE " (Type, K, S, Sat, H, IMM, D, "
                       "ASM_R, CON_R, ENT_R, LUN_R, MPR_R, "
                       "ASM_G, CON_G, ENT_G, LUN_G, MPR_G,"
                       "ASM_B, CON_B, ENT_B, LUN_B, MPR_B,"
                       "ASM_L, CON_L, ENT_L, LUN_L, MPR_L, filename) VALUES ('" + QString::number(type) + "',"
                       "'" + QString::number(object.K) + "', '" + QString::number(object.S) + "',"
                       "'" + QString::number(object.Sat) + "', '" + QString::number(object.H) + "',"
                       "'" + QString::number(object.IMM) + "', '" + QString::number(object.D) + "',"
                       "'" + QString::number(object.ASM_R) + "', '" + QString::number(object.CON_R) + "',"
                       "'" + QString::number(object.ENT_R) + "', '" + QString::number(object.LUN_R) + "',"
                       "'" + QString::number(object.MPR_R) + "', '" + QString::number(object.ASM_G) + "',"
                       "'" + QString::number(object.CON_G) + "', '" + QString::number(object.ENT_G) + "',"
                       "'" + QString::number(object.LUN_G) + "', '" + QString::number(object.MPR_G) + "',"
                       "'" + QString::number(object.ASM_B) + "', '" + QString::number(object.CON_B) + "',"
                       "'" + QString::number(object.ENT_B) + "', '" + QString::number(object.LUN_B) + "',"
                       "'" + QString::number(object.MPR_B) + "', '" + QString::number(object.ASM_L) + "',"
                       "'" + QString::number(object.CON_L) + "', '" + QString::number(object.ENT_L) + "',"
                       "'" + QString::number(object.LUN_L) + "', '" + QString::number(object.MPR_L) + "',"
                       "'" + object.fileName + "')")){
            qDebug() << "Database: error of incert cell in train sample table";
            qDebug() << query.lastError().text();
        }
}

size_t database::getCountMark()
{
    return  typeMarkerName.size();
}

bool database::clearingBaecSetting()
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM " TABLE_BAESSETTING)) {
        qDebug() << "Database: error of clearing probability";
        qDebug() << query.lastError().text();
        return false;
    }
    if (!query.exec("UPDATE SQLITE_SEQUENCE SET SEQ = '0' WHERE NAME = '" TABLE_BAESSETTING "'")){
        qDebug() << "Database: error of updating table probability";
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<std::vector<double> > database::getTrainSample(size_t id)
{
    QSqlQuery query;
    if(!query.exec("SELECT * FROM " TABLE_TRAINSAMPLE " WHERE Type = " +QString::number(id))){
        qDebug() << "Database: error of get train sample for id: " << id;
        qDebug() << query.lastError().text();
        return std::vector<std::vector<double> >();
    }
    std::vector<std::vector<double> > data;
    while (query.next()) {
        std::vector<double> atr;
        for (size_t i = 2; i < getCountMark() + 2; i++) {
            atr.push_back(query.value(i).toDouble());
        }
        data.push_back(atr);
    }
    return data;
}

bool database::insertBaesSetting(size_t idType, size_t idMark, double from, double to, double val)
{
    QSqlQuery query;

    if(!query.exec("INSERT INTO " TABLE_BAESSETTING " (Type, id_" TABLE_TYPEMARK ","
                   "FirstPoint, SecondPoint, Value) VALUES ('" + QString::number(idType) + "', "
                   "'" + QString::number(idMark) + "', '" + QString::number(from) + "', "
                   "'" + QString::number(to) + "', '" + QString::number(val) + "')")) {
        qDebug() << "Database: error of insert probability in the interval from: " << from << " to: " << to;
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

double database::getProbabilty(size_t idType, int idMark, double x)
{
    QSqlQuery query;
    if (!query.exec("SELECT Value FROM " TABLE_BAESSETTING " WHERE Type = " + QString::number(idType) + " "
                    "AND id_" TABLE_TYPEMARK " = " + QString::number(idMark) + " AND FirstPoint <= " + QString::number(x) + " "
                    "AND SecondPoint >= " + QString::number(x))) {
        qDebug() << "Database: error of get probability for type cell: " << idType << " , mark: " << idMark <<
                    " , position" << x;
        qDebug() << query.lastError().text();
        return 0.0;
    }
    double prob = 0.0;
    while (query.next()) {
        prob = query.value(0).toDouble();
    }
    return prob;
}

bool database::initialTableMark()
{
    QSqlQuery query;
    for (auto &mark: typeMarkerName)
        if(!query.exec("INSERT INTO " TABLE_TYPEMARK " (Name) VALUES ('" + mark + "')")){
            qDebug() << query.lastError();
        }
    return true;
}

bool database::openDataBase()
{
    /* База данных открывается по заданному пути
     * и имени базы данных, если она существует
     * */
    //db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(DATABASE_HOSTNAME);
    db.setDatabaseName(".\\" DATABASE_NAME);
    if(db.open()){
        return true;
    } else {
        return false;
    }
}

bool database::restoreDataBase()
{
    if(this->openDataBase()){
           if(!this->createTable()){
               return false;
           } else {
               return true;
           }
       } else {
           qDebug() << "Не удалось восстановить базу данных";
           return false;
       }
}

void database::closeDataBase()
{
    db.close();
}

bool database::createTable()
{
    QSqlQuery query;
    if (!query.exec( "CREATE TABLE " TABLE_TRAINSAMPLE " ("                     //таблица обучающей выборки
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "Type TINYINT  NOT NULL,"
                     "K DOUBLE,"
                     "S DOUBLE,"
                     "Sat DOUBLE,"
                     "H DOUBLE,"
                     "IMM DOUBLE,"
                     "D DOUBLE,"
                     "ASM_R DOUBLE,"
                     "CON_R DOUBLE,"
                     "ENT_R DOUBLE,"
                     "LUN_R DOUBLE,"
                     "MPR_R DOUBLE,"
                     "ASM_G DOUBLE,"
                     "CON_G DOUBLE,"
                     "ENT_G DOUBLE,"
                     "LUN_G DOUBLE,"
                     "MPR_G DOUBLE,"
                     "ASM_B DOUBLE,"
                     "CON_B DOUBLE,"
                     "ENT_B DOUBLE,"
                     "LUN_B DOUBLE,"
                     "MPR_B DOUBLE,"
                     "ASM_L DOUBLE,"
                     "CON_L DOUBLE,"
                     "ENT_L DOUBLE,"
                     "LUN_L DOUBLE,"
                     "MPR_L DOUBLE,"
                     "filename VARCHAR(256));")) {
        qDebug() << "Database: error of create " << TABLE_TRAINSAMPLE;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_TYPEMARK " ("                     //таблица признак
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "Name VARCHAR(64) NOT NULL"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_TYPEMARK;
        qDebug() << query.lastError().text();
        return false;
    }
    if(!query.exec( "CREATE TABLE " TABLE_BAESSETTING " ("        //таблица вероятность попадания в интервал
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "Type TINYINT NOT NULL,"
                    "id_" TABLE_TYPEMARK " INTEGER NOT NULL,"
                    "FirstPoint DOBLE,"
                    "SecondPoint DOUBLE,"
                    "Value DOBLE"
                    ");")){
        qDebug() << "Database: error of create " << TABLE_BAESSETTING;
        qDebug() << query.lastError().text();
        return false;
    }
    initialTableMark();
    return true;
}
