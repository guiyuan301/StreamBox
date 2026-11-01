#ifndef USERMODEL_H
#define USERMODEL_H
#include <QString>
/**
 * @brief 用户业务模型类
 *
 * 该类封装用户相关的数据库操作逻辑，包括：
 * - 用户登录验证
 * - 用户注册
 * - 用户信息查询
 * - 用户信息删除
 *
 * 使用流程：
 * 1. 调用 getInstance() 获取单例实例
 * 2. 调用相关业务方法进行操作
 * 3. 程序结束时调用 destroyInstance() 销毁实例
 */
class UserModel
{
private:
    UserModel();
    UserModel(const UserModel&) = delete ;
    UserModel& operator=(const UserModel&) = delete;

    static UserModel* usermodel ;//用户业务模型层 用于封装用户相关逻辑

    int currentUserID; //当前用户的ID
    QString currentUserName ;//当前用户名
    QString currentUserAccount; //当前用户账户



public:
    static UserModel *getInstance();
    static void destroyInstance();  //删除单例

    bool login(const QString& account, const QString& pwd); //验证用户登录 登录成功后会自动保存用户的信息
    bool registerUser(const QString& account, const QString& pwd, const QString& username); //用户注册 插入新的用户信息
    bool isAccountExist(const QString& account); //判断账户是否存在
    bool deleteAccount(const QString& account);  //删除对应账户的信息
    bool changeUserName(const QString& newUserName,const QString &account);//修改新的用户名称

    QString getCurrentUserName() const; //获取当前用户名
    QString getCurrentAccount() const; //获取当前账户
    int getCurrentUserId() const;  //获取当前用户ID

    void logout();    //用户退出登录
    bool isLogined() const; //检查是否有用户登录
};

#endif // USERMODEL_H
