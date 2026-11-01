#ifndef DBSQLITE3_H
#define DBSQLITE3_H
#include "sqlite3.h"
#include<QString>
/**
 * @brief SQLite数据库封装类
 *
 * 该类封装了SQLite3的C API，提供简洁的数据库操作接口
 * 采用单例模式确保整个程序只有一个数据库连接实例
 *
 * 使用流程：
 * 1. 调用 setFileDbName() 设置数据库文件路径
 * 2. 调用 getInstance() 获取单例实例（自动打开数据库）
 * 3. 调用 doSql() 执行SQL语句
 * 4. 调用 freeResult() 释放查询结果
 * 5. 程序结束时调用 destroyInstance() 销毁实例
 */
class dbSqlite3
{
private:
    dbSqlite3();
    ~dbSqlite3();
    sqlite3* mydb; //数据库句柄
    dbSqlite3(const dbSqlite3&) = delete; //禁用拷贝构造函数
    dbSqlite3& operator=(const dbSqlite3&) = delete; //禁用赋值操作
    static dbSqlite3* mysqliteInstance;//存储整个程序唯一的数据库对象(作为一个储存数据库容器)
    static QString DbFileName; //数据库名称
public:
    static dbSqlite3* getInstance(); //接口初始化数据库
    int databaseClose();  //关闭数据库 返回0 则成功 非0则失败
    int doSql(QString &strsql,char** &presult,int& nrow , int& ncolumn); //执行数据库语句 返回值成功是 >0 失败则是 -1
    static void setFileDbName(const QString dbfileName); //设置数据库名称
    static void destroyInstance(); // 销毁单例实例
    void freeResult(char** presult); //释放doSql() 产生的结果集 每次调用doSql()后必须调用此函数(不管有没有presult都要调用函数会自动判断)
};

#endif // DBSQLITE3_H
