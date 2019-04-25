#include "database.h"
#include <QDebug>

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
                       "ASM_L, CON_L, ENT_L, LUN_L, MPR_L) VALUES ('" + QString::number(type) + "',"
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
                       "'" + QString::number(object.LUN_L) + "', '" + QString::number(object.MPR_L) + "')")){
            qDebug() << "Database: error of incert cell in train sample table";
            qDebug() << query.lastError().text();
        }
}

bool database::openDataBase()
{
    /* База данных открывается по заданному пути
     * и имени базы данных, если она существует
     * */
    db = QSqlDatabase::addDatabase("QSQLITE");
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
                    "MPR_L DOUBLE);")) {
        qDebug() << "Database: error of create " << TABLE_TRAINSAMPLE;
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}
