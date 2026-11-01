#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H
#include <QString>
/**
 * @brief 用户控制器类
 *
 * 该类作为View层和Model层的桥梁，负责：
 * 接收View层的用户操作请求
 * 调用UserModel处理业务逻辑
 * 返回处理结果给View层
 *
 * 使用流程：
 * 1. 调用 getInstance() 获取单例实例
 * 2. 调用相关方法处理用户请求
 * 3. 程序结束时调用 destroyInstance() 销毁实例
 */
class UserController
{
private:
    UserController();
    UserController(const UserController&) = delete;
    UserController& operator=(const UserController&) = delete;

    static UserController* usercontroller;
public:
    static UserController* getInstance();
    static void destroyInstance();

    bool handleLogin(const QString& account, const QString& pwd); //处理用户登录请求
    bool handleRegister(const QString& account, const QString& pwd, const QString& username); //处理用户注册请求
    bool handleDeleteAccount(const QString& account); //处理删除账户请求
    bool handleChangeUserName(const QString& userName,const QString& account);
    void handleLogout(); //处理用户登出请求
    bool checkLoginStatus(); //检查用户是否已登录

    QString getCurrentUserName();//获取当前登录用户名
    QString getCurrentAccount(); //获取当前登录账号
    int getCurrentUserId(); //获取当前用户ID

};

#endif // USERCONTROLLER_H
