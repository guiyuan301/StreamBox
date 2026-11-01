#include <QDebug>
#include "usercontroller.h"
#include "MODEL/usermodel.h"
UserController* UserController::usercontroller = nullptr;
UserController::UserController()
{

}

UserController *UserController::getInstance()
{
    if(usercontroller==nullptr)
    {
        UserController::usercontroller = new UserController();
    }
    return UserController::usercontroller;
}

void UserController::destroyInstance()
{
    if(usercontroller != nullptr)
    {
        delete usercontroller;
        usercontroller = nullptr;
        qDebug() << "[信息] UserController实例已销毁";
    }
}
bool UserController::handleLogin(const QString& account, const QString& pwd)
{
    // 参数校验
    if(account.isEmpty() || pwd.isEmpty())
    {
        qDebug() << "[错误] 账号或密码不能为空";
        return false;
    }

    // 调用Model层处理登录
    UserModel* model = UserModel::getInstance();
    bool result = model->login(account, pwd);

    if(result)
    {
        qDebug() << "[Controller] 登录处理成功";
    }
    else
    {
        qDebug() << "[Controller] 登录处理失败";
    }

    return result;
}
bool UserController::handleRegister(const QString& account, const QString& pwd, const QString& username)
{
    // 参数校验
    if(account.isEmpty() || pwd.isEmpty() || username.isEmpty())
    {
        qDebug() << "[错误] 注册信息不完整";
        return false;
    }

    // 调用Model层处理注册
    UserModel* model = UserModel::getInstance();
    bool result = model->registerUser(account, pwd, username);

    if(result)
    {
        qDebug() << "[Controller] 注册处理成功";
    }
    else
    {
        qDebug() << "[Controller] 注册处理失败";
    }

    return result;
}

bool UserController::handleDeleteAccount(const QString& account)
{
    // 参数校验
    if(account.isEmpty())
    {
        qDebug() << "[错误] 账号不能为空";
        return false;
    }

    // 调用Model层处理删除
    UserModel* model = UserModel::getInstance();
    bool result = model->deleteAccount(account);

    if(result)
    {
        qDebug() << "[Controller] 删除账户处理成功";
    }
    else
    {
        qDebug() << "[Controller] 删除账户处理失败";
    }

    return result;
}

bool UserController::handleChangeUserName(const QString &userName, const QString &account)
{
    if(userName.isEmpty())
    {
        qDebug() << "[错误] 新用户名不能为空";
        return false;
    }
    //调用Model层处理修改用户名
    UserModel* model = UserModel::getInstance();
    bool result = model->changeUserName(userName,account);
    if(result)
    {
        qDebug() << "[Controller] 修改用户名称成功";
    }
    else
    {
        qDebug() << "[Controller] 修改用户名称失败";
    }
    return result;
}

void UserController::handleLogout()
{
    UserModel* model = UserModel::getInstance();
    model->logout();
    qDebug() << "[Controller] 登出处理完成";
}

bool UserController::checkLoginStatus()
{
    UserModel* model = UserModel::getInstance();
    return model->isLogined();
}

QString UserController::getCurrentUserName()
{
    UserModel* model = UserModel::getInstance();
    return model->getCurrentUserName();
}

QString UserController::getCurrentAccount()
{
    UserModel* model = UserModel::getInstance();
    return model->getCurrentAccount();
}

int UserController::getCurrentUserId()
{
    UserModel* model = UserModel::getInstance();
    return model->getCurrentUserId();
}
