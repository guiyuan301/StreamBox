#include "usermodel.h"
#include "DBdate/dbsqlite3.h"
#include <QDebug>
//初始化静态变量
UserModel* UserModel::usermodel = nullptr;


UserModel *UserModel::getInstance()
{
    if(usermodel==nullptr)
    {
        UserModel::usermodel = new UserModel();
    }
    return UserModel::usermodel;
}

UserModel::UserModel() :currentUserID(-1),currentUserName(""),currentUserAccount("")
{   
}

void UserModel::destroyInstance()
{
    if(usermodel != nullptr)
    {
        delete usermodel;
        usermodel = nullptr;
        qDebug() << "[信息] UserModel实例已销毁";
    }
}

bool UserModel::login(const QString &account, const QString &pwd)
{
    dbSqlite3* db =dbSqlite3::getInstance();
    QString strsql = QString ("select user_id,username from user where account = '%1' and password = '%2';")
            .arg(account).arg(pwd);
    char**presult = nullptr;
    int nrow=0 ,ncolumn =0;
    int res = db->doSql(strsql,presult,nrow,ncolumn);
    //!注意 因为是select 所以结果必须是>0 才算是成功查询其他的例如删除 插入新数据 一般返回0因为无返回数据
    if(res >0)
    {
        currentUserID = QString(presult[ncolumn]).toInt();
        currentUserName = presult[ncolumn+1];
        currentUserAccount = account ;
        qDebug()<< "[成功] 登录成功，用户:" << currentUserName;
        db->freeResult(presult);
        return true;
    }

    else
    {
        qDebug() << "[失败] 登录失败，账号或密码错误";
        db->freeResult(presult);
        return false;
    }
}

bool UserModel::registerUser(const QString &account, const QString &pwd, const QString &username)
{
    //先判断账户是否存在
    if(isAccountExist(account))
    {
        qDebug() << "[失败] 账号已存在";
        return false;
    }
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("insert into user(account,password,username)values('%1','%2','%3')")
            .arg(account).arg(pwd).arg(username);
    char** presult = nullptr;
    int nrow=0 ,ncolumn =0;
    int res = db->doSql(strsql,presult,nrow,ncolumn);
    db->freeResult(presult);
    if (res >=0)
    {
        qDebug() << "[成功] 注册成功";
        return true;
    }
    qDebug() << "[失败] 注册失败";
    return false;

}

bool UserModel::isAccountExist(const QString &account)
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("select user_id from user where account = '%1' ;").arg(account);
    char** presult = nullptr;
    int nrow =0,ncolumn = 0;
    int res =db->doSql(strsql,presult,nrow,ncolumn);
    db->freeResult(presult);
    return res >0; //如果为真 返回true 假为 false
}

bool UserModel::deleteAccount(const QString &account)
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("delete from user where account = '%1';").arg(account);
    char** presult = nullptr ;
    int nrow=0 ,ncolumn =0;
    int res = db->doSql(strsql,presult,nrow,ncolumn);
    db->freeResult(presult);
    if(res>=0)
    {
        qDebug() << "[成功] 账号删除成功";
        return true;
    }
    qDebug() << "[失败] 账号删除失败";
    return false;
}

bool UserModel::changeUserName(const QString &newUserName,const QString &account)
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("UPDATE user set username = '%1' where account = '%2';").arg(newUserName).arg(account);
    char** presult = nullptr ;
    int nrow=0 ,ncolumn =0;
    int res = db->doSql(strsql,presult,nrow,ncolumn);
    db->freeResult(presult);
    if(res>=0)
    {
        qDebug() << "[成功] 用户名称修改成功";
        currentUserName = newUserName;
        return true;
    }
    qDebug() << "[失败] 用户名称修改失败";
    return false;
}

QString UserModel::getCurrentUserName() const
{
    return currentUserName;
}

QString UserModel::getCurrentAccount() const
{
    return currentUserAccount;
}

int UserModel::getCurrentUserId() const
{
    return currentUserID;
}

void UserModel::logout()
{
    currentUserID = -1;
    currentUserAccount = "";
    currentUserName = "";
    qDebug() << "[信息] 用户已登出";
}

bool UserModel::isLogined() const
{
    return currentUserID > 0;
}



