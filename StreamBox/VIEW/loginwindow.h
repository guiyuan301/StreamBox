#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>


class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    void showWithAnimation();
    signals:
    void loginSuccess(); // 登录成功信号

protected:
    // 重写鼠标事件以实现无边框窗口的拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onRegisterSuccess(); // 注册成功后的特效槽函数

private:
    void initUI();            // 初始化UI布局
    void initStyle();         // 初始化QSS样式
    void initConnections();   // 绑定信号槽

    QWidget *createLoginPage();    // 创建登录页
    QWidget *createRegisterPage(); // 创建注册页

private:
    QStackedWidget *m_stackedWidget; //页面堆叠容器

    // 登录页控件
    QLineEdit *m_loginUserEdit;
    QLineEdit *m_loginPwdEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_toRegBtn;
    QLabel *m_errorLabel;

    // 注册页控件
    QLineEdit *m_regUserEdit;  //Account 账户
    QLineEdit *m_regPwdEdit;
    QLineEdit *m_regConfirmPwdEdit;
    QPushButton *m_regBtn;
    QPushButton *m_toLoginBtn;


    // 窗口拖动相关
    bool m_isDragging;   //是否拖动窗口
    QPoint m_dragPosition; //保存鼠标对于窗口的偏移位置
};

#endif // LOGINWINDOW_H
