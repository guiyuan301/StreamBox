#include "loginwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QApplication>
#include "CONTROLLER/usercontroller.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), m_isDragging(false)
{
    // 1. 设置无边框窗口和透明背景（实现圆角的关键）
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(400, 520); // 设定窗口大小

    initUI();
    initStyle();
    initConnections();
}

LoginWindow::~LoginWindow() {}

void LoginWindow::initUI()
{

    // 添加Window图标
    setWindowIcon(QIcon(":/pic/StreamBox.png"));
    // 最外层布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10); // 留出边缘给阴影或圆角

    // 核心背景容器 (用来承载颜色和圆角)
    QWidget *bgWidget = new QWidget(this);
    bgWidget->setObjectName("BgWidget");
    mainLayout->addWidget(bgWidget);

    // 为背景容器添加阴影效果 (提升精致感)
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setBlurRadius(15);
    bgWidget->setGraphicsEffect(shadow);

    QVBoxLayout *bgLayout = new QVBoxLayout(bgWidget);
    bgLayout->setContentsMargins(20, 10, 20, 20);

    // 顶部栏：包含自定义的关闭按钮
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    QPushButton *closeBtn = new QPushButton("×", this);
    closeBtn->setObjectName("CloseBtn");
    closeBtn->setFixedSize(30, 30);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QApplication::quit); //点击这个按钮之后关闭整个程序

    topBarLayout->addStretch();
    topBarLayout->addWidget(closeBtn);
    bgLayout->addLayout(topBarLayout);

    // 实例化页面切换容器
    m_stackedWidget = new QStackedWidget(this);
    bgLayout->addWidget(m_stackedWidget);

    // 添加登录页和注册页
    m_stackedWidget->addWidget(createLoginPage());
    m_stackedWidget->addWidget(createRegisterPage());

}

QWidget* LoginWindow::createLoginPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 0, 20, 20);

    // 标题区域
    QLabel *titleLabel = new QLabel("StreamBox", this);
    titleLabel->setObjectName("TitleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *subTitle = new QLabel("欢迎回来，开启你的影音之旅", this);
    subTitle->setObjectName("SubTitle");
    subTitle->setAlignment(Qt::AlignCenter);

    // 输入框
    m_loginUserEdit = new QLineEdit(this);
    m_loginUserEdit->setPlaceholderText("请输入用户名");

    m_loginPwdEdit = new QLineEdit(this);
    m_loginPwdEdit->setPlaceholderText("请输入密码");
    m_loginPwdEdit->setEchoMode(QLineEdit::Password);

    // 按钮
    m_loginBtn = new QPushButton("登 录", this);
    m_loginBtn->setObjectName("PrimaryBtn");
    m_loginBtn->setCursor(Qt::PointingHandCursor);

    m_toRegBtn = new QPushButton("没有账号？立即注册", this);
    m_toRegBtn->setObjectName("SwitchBtn");
    m_toRegBtn->setCursor(Qt::PointingHandCursor);

    // 实例化错误提示标签，设置样式并默认隐藏
    m_errorLabel = new QLabel("", this);
    m_errorLabel->setStyleSheet("color: #FF4757; font-size: 12px; background: transparent;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide(); // 默认隐藏
    // 组装
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(subTitle);
    layout->addSpacing(10);
    layout->addWidget(m_loginUserEdit);
    layout->addWidget(m_loginPwdEdit);
    layout->addWidget(m_errorLabel);
    layout->addSpacing(10);
    layout->addWidget(m_loginBtn);
    layout->addWidget(m_toRegBtn);
    layout->addStretch();

    return page;
}

QWidget* LoginWindow::createRegisterPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 0, 20, 20);

    // 标题区域
    QLabel *titleLabel = new QLabel("创建账号", this);
    titleLabel->setObjectName("TitleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    // 输入框
    m_regUserEdit = new QLineEdit(this);
    m_regUserEdit->setPlaceholderText("设置用户名");

    m_regPwdEdit = new QLineEdit(this);
    m_regPwdEdit->setPlaceholderText("设置密码");
    m_regPwdEdit->setEchoMode(QLineEdit::Password);

    m_regConfirmPwdEdit = new QLineEdit(this);
    m_regConfirmPwdEdit->setPlaceholderText("确认密码");
    m_regConfirmPwdEdit->setEchoMode(QLineEdit::Password);


    // 按钮
    m_regBtn = new QPushButton("注 册", this);
    m_regBtn->setObjectName("PrimaryBtn");
    m_regBtn->setCursor(Qt::PointingHandCursor);

    m_toLoginBtn = new QPushButton("已有账号？直接登录", this);
    m_toLoginBtn->setObjectName("SwitchBtn");
    m_toLoginBtn->setCursor(Qt::PointingHandCursor);

    // 组装
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(m_regUserEdit);
    layout->addWidget(m_regPwdEdit);
    layout->addWidget(m_regConfirmPwdEdit);
    layout->addSpacing(10);
    layout->addWidget(m_regBtn);
    layout->addWidget(m_toLoginBtn);
    layout->addStretch();

    return page;
}

void LoginWindow::initConnections()
{
    // 页面互相切换的逻辑
    connect(m_toRegBtn, &QPushButton::clicked, [=]()
    {
        m_stackedWidget->setCurrentIndex(1);
    });
    connect(m_toLoginBtn, &QPushButton::clicked, [=]()
    {
        m_stackedWidget->setCurrentIndex(0);
    });

    // 绑定注册按钮的特效
    connect(m_regBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterSuccess);

    // 绑定登录逻辑
    connect(m_loginBtn, &QPushButton::clicked, this, [=]() {
    QString user = m_loginUserEdit->text();
    QString pwd = m_loginPwdEdit->text();
    UserController* usercontroller = UserController::getInstance(); //接入Controller层

    //进行登录判断
    if (usercontroller->handleLogin(user,pwd))
    {
        m_errorLabel->hide();
        m_loginBtn->setText("登录中...");
        m_loginBtn->setEnabled(false);

        // 登录成功：当前窗口淡出，发射成功信号
        QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "windowOpacity");
        fadeOut->setDuration(500);
        fadeOut->setEndValue(0.0);
        connect(fadeOut, &QPropertyAnimation::finished, this, [=]()
        {
            this->hide();
            emit loginSuccess(); // 通知主程序打开主页面
        });
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        m_errorLabel->setText("账号或密码错误，请重试！");
        m_errorLabel->show();
        // 给输入框加一个红色边框警告
        m_loginPwdEdit->setStyleSheet("border: 1px solid #FF4757;");
    }
    });

    // 让用户按回车键也能直接登录
    connect(m_loginUserEdit, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);
    connect(m_loginPwdEdit, &QLineEdit::returnPressed, m_loginBtn, &QPushButton::click);
}

// 核心特效：渐隐消失 -> 切换页面 -> 渐显出现
void LoginWindow::onRegisterSuccess()
{
    //注册判断
    UserController* usercontroller = UserController::getInstance();
    bool res = usercontroller->handleRegister(m_regUserEdit->text(),m_regPwdEdit->text(),"newUser"); //默认用户名为newUser 随后可以在设置
    if(res==false)
    {
        //这里需要优化 :不太好看
        m_regUserEdit->setText("注册失败!");
        return ;
    }
    // 禁用按钮防多次点击
    m_regBtn->setEnabled(false);

    // 淡出动画
    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "windowOpacity");
    fadeOut->setDuration(500); // 500毫秒
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InOutQuad);

    connect(fadeOut, &QPropertyAnimation::finished, this, [=]()
    {
    // 动画结束后：切换到登录页
    m_stackedWidget->setCurrentIndex(0); //登陆界面 0 注册界面1

    // 自动将刚刚注册的用户名填入登录框
    m_loginUserEdit->setText(m_regUserEdit->text());
    m_loginPwdEdit->clear(); // 清空密码框让用户重输
    m_errorLabel->hide();    // 顺便隐藏之前的错误提示

    // 清空注册页的数据
    m_regUserEdit->clear();
    m_regPwdEdit->clear();
    m_regConfirmPwdEdit->clear();

    // 2. 淡入动画
    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(500);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    m_regBtn->setEnabled(true);
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

// ================= 全局 QSS 样式 ===========================
void LoginWindow::initStyle()
{
    // 使用 C++11 的原始字符串 R"(...)"，写 QSS 很方便
    QString qss = R"(
    /* 基础容器 */
    #BgWidget {
        background-color: #1E1E24;
        border-radius: 12px;
        border: 1px solid #2D2D3B;
    }

    /* 标题文字 */
    #TitleLabel {
        color: #00ADB5;
        font-size: 26px;
        font-weight: 900;
        font-family: "Microsoft YaHei";
    }
    #SubTitle {
        color: #808090;
        font-size: 13px;
    }

    /* 输入框 */
    QLineEdit {
        background-color: #141419;
        border: 1px solid #2D2D3B;
        border-radius: 6px;
        padding: 12px 15px;
        color: #FFFFFF;
        font-size: 14px;
        font-family: "Microsoft YaHei";
    }
    QLineEdit:focus {
        border: 1px solid #00ADB5;
        background-color: #1A1A20;
    }

    /* 主按钮 (登录/注册) */
    #PrimaryBtn {
        background-color: #00ADB5;
        color: #FFFFFF;
        border: none;
        border-radius: 6px;
        padding: 12px;
        font-size: 16px;
        font-weight: bold;
        font-family: "Microsoft YaHei";
    }
    #PrimaryBtn:hover {
        background-color: #00C6D0;
    }
    #PrimaryBtn:pressed {
        background-color: #00939A;
    }

    /* 文字切换按钮 */
    #SwitchBtn {
        background-color: transparent;
        color: #808090;
        border: none;
        font-size: 13px;
    }
    #SwitchBtn:hover {
        color: #00ADB5;
        text-decoration: underline;
    }

    /* 关闭按钮 */
    #CloseBtn {
        background-color: transparent;
        color: #808090;
        font-size: 20px;
        font-weight: bold;
        border: none;
    }
    #CloseBtn:hover {
        color: #FF4757; /* 悬浮变红 */
    }
    )";

    this->setStyleSheet(qss);
}

// ================= 鼠标拖动无边框窗口逻辑 ================================
void LoginWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isDragging = true;
        m_dragPosition = event->globalPos() - this->frameGeometry().topLeft();//让窗口跟随鼠标顺滑拖
        event->accept();
    }
}

void LoginWindow::mouseMoveEvent(QMouseEvent *event)
{
    //&是鼠标和按键专属的东西 专门用于 Qt 按键判断(运用的是位运算逻辑)
    if (event->buttons() & Qt::LeftButton && m_isDragging)
    {
        //根据偏移量平滑移动 (没有偏移量的话就会出现突然闪现到对应位置)
        this->move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void LoginWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    event->accept();
}

// 淡入显示方法
void LoginWindow::showWithAnimation()
{
    this->setWindowOpacity(0.0);
    this->show();

    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(600);
    fadeIn->setEndValue(1.0);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

}
