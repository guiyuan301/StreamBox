#include "splashscreen.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QIcon>

SplashScreen::SplashScreen(QWidget *parent) : QWidget(parent), m_progressValue(0)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(500, 300);

    // 基础容器
    QWidget *bgWidget = new QWidget(this);
    bgWidget->setObjectName("SplashBg");

    // 添加阴影
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(0, 0);
    bgWidget->setGraphicsEffect(shadow);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->addWidget(bgWidget);

    QVBoxLayout *bgLayout = new QVBoxLayout(bgWidget);

    // Logo 和 标语
    QLabel *logoLabel = new QLabel("StreamBox", this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("color: #00ADB5; ");
    QFont logoFont("Microsoft YaHei", 24, QFont::Black); // Black 相当于 weight 900
    logoFont.setLetterSpacing(QFont::AbsoluteSpacing, 5); // 设置绝对字符间距为 5px
    logoLabel->setFont(logoFont);

    QLabel *subLabel = new QLabel("开启你的极致影音体验...", this);
    subLabel->setAlignment(Qt::AlignCenter);
    subLabel->setStyleSheet("color: #808090; font-size: 14px; margin-bottom: 20px;");

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setFixedHeight(4); // 极简细线进度条
    m_progressBar->setTextVisible(false);

    bgLayout->addStretch();
    bgLayout->addWidget(logoLabel);
    bgLayout->addWidget(subLabel);
    bgLayout->addWidget(m_progressBar);
    bgLayout->addStretch();

    // 样式
    bgWidget->setStyleSheet(R"(
        #SplashBg {
            background-color: #1E1E24;
            border-radius: 15px;
            border: 1px solid #2D2D3B;
        }
        QProgressBar {
            background-color: #141419;
            border-radius: 2px;
        }
        QProgressBar::chunk {
            background-color: #00ADB5;
            border-radius: 2px;
        }
    )");
}

void SplashScreen::showWithAnimation() {
    //设置动画的WindowIcon
    setWindowIcon(QIcon(":/pic/StreamBox.png"));
    this->setWindowOpacity(0.0); //必须将窗口设置为完全透明,不然无法达到淡入的效果
    this->show();

    // 淡入动画
    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity"); //"windowOpacity" 表示修改的属性是透明度
    fadeIn->setDuration(800);
    fadeIn->setEndValue(1.0); //设置值 ,因为从无到最清晰是 0.0-1.0 如果旋转或平移可以在定
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped); //DeleteWhenStopped 删除动画当动画结束

    // 开始模拟加载
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SplashScreen::updateProgress);
    timer->start(30); // 每30ms更新一次进度
}

void SplashScreen::updateProgress() {
    m_progressValue += 2;
    m_progressBar->setValue(m_progressValue);

    if (m_progressValue >= 100)
    {
        sender()->deleteLater(); // 停止并销毁定时器

        // 加载完成后，淡出动画
        QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "windowOpacity");
        fadeOut->setDuration(600);
        fadeOut->setEndValue(0.0);

        connect(fadeOut, &QPropertyAnimation::finished, this, [=]()
        {
            this->hide();
            emit finished(); // 触发结束信号
        });

        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    }
}
