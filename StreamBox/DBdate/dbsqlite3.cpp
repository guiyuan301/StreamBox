#include "dbsqlite3.h"
#include <QDebug>
//初始化静态成员
dbSqlite3* dbSqlite3::mysqliteInstance = nullptr;
QString dbSqlite3::DbFileName = "";



dbSqlite3::dbSqlite3() :mydb (nullptr)
{
    //检查数据库文件路径是否配置
    if(DbFileName.isEmpty())
    {
        qDebug()<<"错误!数据库文件路径未设置，请先调用 setFileDbName()";
        return  ;
    }

    int res = sqlite3_open(dbSqlite3::DbFileName.toLatin1(),&this->mydb);
    if(res !=SQLITE_OK)
    {
        qDebug()<< "数据库打开失败!!错误码"<<res;
        //打开失败后应将mydb清空并设置为nullptr
        if(mydb!=nullptr)
        {
            sqlite3_close(mydb);
            mydb = nullptr;
        }
        return ;
    }
    qDebug() << "[成功] 数据库已打开:" << DbFileName;
}

dbSqlite3::~dbSqlite3()
{
    if(this->mydb!=nullptr)
    {
       sqlite3_close(mydb);
       mydb = nullptr;
       qDebug() << "[信息] 数据库连接已关闭";
    }
}

dbSqlite3 *dbSqlite3::getInstance()
{
    if(mysqliteInstance==nullptr)
    {
        dbSqlite3::mysqliteInstance=new dbSqlite3(); //将mysqliteInstance 作为储存数据库的容器
    }
    return  dbSqlite3::mysqliteInstance;
}

void dbSqlite3::destroyInstance()
{
    // 检查实例是否存在
    if(mysqliteInstance != nullptr)
    {
        // 删除实例（会自动调用析构函数关闭数据库）
        delete mysqliteInstance;
        // 将指针置空，防止悬空指针
        mysqliteInstance = nullptr;
        qDebug() << "[信息] 数据库实例已销毁";
    }
}

int dbSqlite3::databaseClose()
{
    //检查是否打开数据库
    if(this->mydb ==nullptr)
    {
        qDebug() << "[警告] 数据库未打开";
        return -1;
    }
    //关闭数据库连接
    int res = sqlite3_close(this->mydb);
    if(res == SQLITE_OK)
    {
        qDebug() << "[成功] 数据库关闭成功！";
        mydb = nullptr;
        return 0; //返回0 代表成功
    }
    else
    {
        qDebug() << "[错误] 数据库关闭失败，错误码：" << res;
        return res;
    }
}

int dbSqlite3::doSql(QString &strsql, char **&presult, int &nrow, int &ncolumn)
{
    char* errmsg = nullptr;
    if(this->mydb == nullptr)
    {
        qDebug() << "[错误] 数据库未打开，无法执行SQL";
        return -1;
    }
    int res = sqlite3_get_table(this->mydb,strsql.toLatin1(),&presult,&nrow,&ncolumn,&errmsg);
    if(res ==SQLITE_OK)
    {
        if(errmsg != nullptr)
        {
            sqlite3_free(errmsg);  //释放错误信息内存
            errmsg = nullptr;
        }
        return  nrow; //成功,返回行数 一般是>1 因为表有表头和数据 所有一般是>1 有数据即代表执行成功
    }
    else
    {
        qDebug() << "[错误] SQL执行失败! 错误码:" << res;
        if(errmsg != nullptr)
        {
            qDebug() << "[错误] 错误信息:" << errmsg;
            sqlite3_free(errmsg);
            errmsg = nullptr;
        }
        return -1;
    }
}

void dbSqlite3::setFileDbName(const QString dbfileName)
{
    dbSqlite3::DbFileName = dbfileName;
}


void dbSqlite3::freeResult(char** presult)
{
    // 检查指针是否有效
    if(presult != nullptr)
    {
        // 释放sqlite3_get_table分配的内存
        sqlite3_free_table(presult);
        qDebug() << "[信息] 结果集内存已释放";
    }
}

