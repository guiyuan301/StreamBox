#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include"CONTROLLER/videocontroller.h"
#include "videocardwidget.h"
#include "CONTROLLER/usercontroller.h"
#include <QFrame>
#include <QPushButton>
#include <QCheckBox>
#include <QCompleter>
#include <QStringList>
#include <QAbstractItemView>
#include <QInputDialog>
#include <QLineEdit>
#include "loginwindow.h"
#include "splashscreen.h"
#include <QDir>
#include <QScrollBar>
#include <QDateTime>
#include <QCoreApplication>
#include <QCoreApplication>
#include "Video/livevideoaudio.h"
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_isDragging(false), m_playWindow(nullptr) {

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->resize(1000, 650); // 初始大窗口

    initUI();
    initStyle();
}
MainWindow::~MainWindow()
{
    if(m_playWindow)
    {
        m_playWindow->close();
        m_playWindow = nullptr;
    }
}
void MainWindow::initUI() {
    // 1. 最外层主容器 (为了阴影留边)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QWidget *bgWidget = new QWidget(this);
    bgWidget->setObjectName("MainBg");
    mainLayout->addWidget(bgWidget);

    // 添加Window图标
    setWindowIcon(QIcon(":/pic/StreamBox.png"));
    // 添加高级窗口阴影
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 180));
    bgWidget->setGraphicsEffect(shadow);

    // 2. 将主界面分为左右两部分
    QHBoxLayout *contentLayout = new QHBoxLayout(bgWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // ================= 左侧导航栏 =================
    QWidget *leftSidebar = new QWidget(this);
    leftSidebar->setObjectName("LeftSidebar");
    leftSidebar->setFixedWidth(200);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftSidebar);
    leftLayout->setContentsMargins(0, 30, 0, 30);
    leftLayout->setSpacing(10);

    QLabel *logoLabel = new QLabel("StreamBox", this);
    logoLabel->setObjectName("LogoLabel");
    logoLabel->setAlignment(Qt::AlignCenter);
    QFont logoFont("Microsoft YaHei", 16, QFont::Black);
    logoFont.setLetterSpacing(QFont::AbsoluteSpacing, 2); // 设置绝对字符间距为 2px
    logoLabel->setFont(logoFont);
    leftLayout->addWidget(logoLabel);
    leftLayout->addSpacing(30);

    // 实例化导航按钮组
    m_menuGroup = new QButtonGroup(this);
    m_menuGroup->setExclusive(true); // 互斥，同一时间只能选中一个

    // 侧边栏按钮 (我的、设置)
    HoverNavButton *mineBtn = new HoverNavButton("👤 我的账户", this);
    HoverNavButton *settingBtn = new HoverNavButton("⚙️ 系统设置", this);
    mineBtn->setCheckable(true);
    settingBtn->setCheckable(true);

    m_menuGroup->addButton(mineBtn, 2);
    m_menuGroup->addButton(settingBtn, 3);

    leftLayout->addWidget(mineBtn);
    leftLayout->addWidget(settingBtn);
    leftLayout->addStretch(); // 弹簧顶上去

    // ================= 右侧主区域 =================
    QWidget *rightContainer = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // 右侧顶部栏 (包含直播、热门、影视、搜索框)
    QWidget *topBar = new QWidget(this);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(70);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(30, 0, 20, 0);
    // ----------------- 右侧顶部栏按钮 -----------------
    QPushButton *liveBtn = new QPushButton("直播", this);
    QPushButton *movieBtn = new QPushButton("影视", this);
    liveBtn->setObjectName("TopNavBtn");
    movieBtn->setObjectName("TopNavBtn");
    // 默认选中直播
    liveBtn->setCheckable(true);
    movieBtn->setCheckable(true);
    liveBtn->setChecked(true);
    // 重新分配按钮组 ID
    m_menuGroup->addButton(liveBtn, 0);
    m_menuGroup->addButton(movieBtn, 1);

    topLayout->addWidget(liveBtn);
    topLayout->addWidget(movieBtn);
    topLayout->addStretch(); // 撑开中间

    // 添加动画搜索框
    AnimatedSearchBox *searchBox = new AnimatedSearchBox(this);
    topLayout->addWidget(searchBox, 0, Qt::AlignVCenter);
    // ================== 新增：搜索补全及跳转功能 ==================
    // 1. 从数据库获取所有视频，提取名字构建关键词列表
    QList<VideoInfo> allVideos = VideoController::getInstance()->getAllVideos();
    QStringList videoNames;
    for(const VideoInfo& info : allVideos)
    {
        videoNames << info.videoName;
    }
    // 2. 创建补全器 (QCompleter)
    QCompleter *completer = new QCompleter(videoNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); // 忽略大小写
    completer->setFilterMode(Qt::MatchContains);        // 只要包含输入字就显示
    completer->setMaxVisibleItems(6);                   // 下拉框最多显示条目数
    // 3. 美化下拉列表样式 (保持暗黑风格)
    completer->popup()->setStyleSheet(
        "QListView {"
        "   background-color: #1A1A20;"
        "   color: #FFFFFF;"
        "   border: 1px solid #3D3D4D;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "   outline: none;" // 隐藏虚线框
        "}"
        "QListView::item {"
        "   padding: 10px;"
        "}"
        "QListView::item:hover, QListView::item:selected {"
        "   background-color: #00ADB5;" /* 主题色高亮 */
        "   color: #FFFFFF;"
        "}"
    );
    // 4. 将补全器绑定到搜索框上 (前提: AnimatedSearchBox 继承自 QLineEdit)
    searchBox->setCompleter(completer);
    // 核心切换逻辑：监听按钮组点击，切换 StackedWidget 索引
        connect(m_menuGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id){
            m_stackedWidget->setCurrentIndex(id);
            if(id == 1) { // 影视页面
                loadMovieVideos();
            }
            else if (id == 2) {
                // 【修复数据为空】每次点击“我的账户”时，动态重新去拉取最新数据
                QLabel* nameLbl = this->findChild<QLabel*>("AccountNameLabel");
                QLabel* accLbl = this->findChild<QLabel*>("AccountIdLabel");

                if(nameLbl && accLbl) {
                    QString userName = UserController::getInstance()->getCurrentUserName();
                    int userId = UserController::getInstance()->getCurrentUserId();
                    QString account = UserController::getInstance()->getCurrentAccount();

                    if (userName.isEmpty()) userName = "访客用户";
                    if (account.isEmpty()) account = "---";

                    nameLbl->setText(userName);
                    accLbl->setText(QString("用户 ID: %1   |   账号: %2").arg(userId).arg(account));
                }
            }
        });
    // ==============================================================
    // 添加关闭按钮
    topLayout->addSpacing(20);
    QPushButton *closeBtn = new QPushButton("×", this);
    closeBtn->setObjectName("CloseBtn");
    closeBtn->setFixedSize(30, 30);
    connect(closeBtn, &QPushButton::clicked, this, [=](){
        QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "windowOpacity");
        fadeOut->setDuration(300);
        fadeOut->setEndValue(0.0);
        connect(fadeOut, &QPropertyAnimation::finished, this, [=](){
            emit closedSignal();
        });
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    });

    topLayout->addWidget(closeBtn, 0, Qt::AlignVCenter);

    // 右侧下部内容区 (StackedWidget)
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName("ContentArea");

    m_stackedWidget->addWidget(createLivePage());                    // index 0
    m_stackedWidget->addWidget(createMoviePage());                   // index 1
    m_stackedWidget->addWidget(createAccountPage());                 // index 2
    m_stackedWidget->addWidget(createSettingsPage());                // index 3

    // 组装右侧
    rightLayout->addWidget(topBar);
    rightLayout->addWidget(m_stackedWidget);

    // 组装全部
    contentLayout->addWidget(leftSidebar);
    contentLayout->addWidget(rightContainer);

    // 核心切换逻辑：监听按钮组点击，切换 StackedWidget 索引
    connect(m_menuGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id){
        m_stackedWidget->setCurrentIndex(id);
        // 影视页面的 id 为 1
        if(id == 1) {
            loadMovieVideos();
        }
    });
}
    // 辅助函数：快速生成带居中文字颜色的测试页面
QWidget* MainWindow::createPage(const QString &title, const QString &bgColor)
{
    QWidget *page = new QWidget();
    page->setStyleSheet(QString("background-color: %1;").arg(bgColor));
    QVBoxLayout *layout = new QVBoxLayout(page);
    QLabel *label = new QLabel(title, page);
    label->setStyleSheet("color: #FFFFFF; font-size: 36px; font-weight: bold; background: transparent;");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    return page;
}
// ============================================================================
// 【新增】创建直播中心页面
// ============================================================================
QWidget* MainWindow::createLivePage()
{
    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #141419;");
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel *iconLabel = new QLabel("📡", page);
    iconLabel->setStyleSheet("font-size: 80px; background: transparent; border: none;");
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel("直播控制中心", page);
    titleLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold; margin-top: 20px; background: transparent;");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *subLabel = new QLabel("准备就绪，随时向世界分享您的精彩瞬间", page);
    subLabel->setStyleSheet("color: #888888; font-size: 14px; margin-bottom: 50px; background: transparent;");
    subLabel->setAlignment(Qt::AlignCenter);

    QPushButton *startLiveBtn = new QPushButton("开 启 直 播", page);
    startLiveBtn->setFixedSize(220, 55);
    startLiveBtn->setCursor(Qt::PointingHandCursor);
    startLiveBtn->setStyleSheet(
        "QPushButton { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FF512F, stop:1 #DD2476); color: white; border-radius: 27px; font-size: 18px; font-weight: bold; border: none; }"
        "QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FF6140, stop:1 #E03585); }"
    );

    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subLabel);
    mainLayout->addWidget(startLiveBtn, 0, Qt::AlignHCenter);

    // ================= 开启直播逻辑 =================
    connect(startLiveBtn, &QPushButton::clicked, this, [=]() {
        QWidget *liveWindow = new QWidget();
        liveWindow->setAttribute(Qt::WA_DeleteOnClose);
        liveWindow->setWindowTitle("StreamBox - 正在直播");
        liveWindow->resize(960, 640);
        liveWindow->setStyleSheet("background-color: #000000;");

        QVBoxLayout *liveLayout = new QVBoxLayout(liveWindow);

        QLabel *videoRenderArea = new QLabel("摄像头加载中...", liveWindow);
        videoRenderArea->setStyleSheet("color: #888888; font-size: 20px;");
        videoRenderArea->setAlignment(Qt::AlignCenter);

        QPushButton *stopLiveBtn = new QPushButton("结束直播并生成回放", liveWindow);
        stopLiveBtn->setFixedSize(220, 50);
        stopLiveBtn->setCursor(Qt::PointingHandCursor);
        stopLiveBtn->setStyleSheet("QPushButton { background-color: #E53935; color: white; border-radius: 25px; font-size: 16px; font-weight: bold; }"
                                   "QPushButton:hover { background-color: #EF5350; }");

        liveLayout->addWidget(videoRenderArea, 1);
        liveLayout->addWidget(stopLiveBtn, 0, Qt::AlignHCenter);

        // ---------------- 启动你的解码流 ----------------
        VideoDecode *videoThread = new VideoDecode();
        VideoDecode::m_WichChannel = 1; // 1代表直播摄像头

        AudioDecode *audioThread = new AudioDecode();
        AudioDecode::m_Wichchannel = 1; // 1代表麦克风

        // 渲染摄像头画面到界面
        connect(videoThread, &VideoDecode::sendImg, videoRenderArea, [=](QImage img){
            videoRenderArea->setPixmap(QPixmap::fromImage(img).scaled(videoRenderArea->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        });

        videoThread->start();
        audioThread->start();

        // ---------------- 结束直播逻辑 ----------------
        connect(stopLiveBtn, &QPushButton::clicked, liveWindow, [=](){
            stopLiveBtn->setEnabled(false);
            stopLiveBtn->setText("正在合成录像中...");
            QApplication::processEvents(); // 刷新一下UI状态

            // 1. 安全停止你的线程
            videoThread->stop();
            audioThread->stop();
            videoThread->wait();
            audioThread->wait();
            delete videoThread;
            delete audioThread;

            //结束直播后将通道设置为视频频道
            VideoDecode::m_WichChannel = 0;
            AudioDecode::m_Wichchannel = 0;

            // 2. 准备输出路径 (bin目录下的 dtbase_path/video_path)
            QString binPath = QCoreApplication::applicationDirPath();
            QString targetPath = binPath + "/dtbase_path/video_path";
            QDir dir;
            if(!dir.exists(targetPath)) {
                dir.mkpath(targetPath); // 如果文件夹不存在则自动创建
            }

            // 拼接最终的 MP4 文件名
            QString outputName = targetPath + "/LiveRecord_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".mp4";
            QString aacPath = binPath + "/temp.aac";
            QString h264Path = binPath + "/temp.h264";

            // 3. 调用合成模块
            liveVideoAudio synthesizer;
            synthesizer.h264AndAacToMp4(aacPath, h264Path, outputName);

            QMessageBox::information(nullptr, "提示", "直播已结束，回放文件已成功保存至:\n" + outputName);
            liveWindow->close();
        });

        liveWindow->show();
        liveWindow->raise();
    });

    return page;
}

// ============================================================================
//   创建影视页面（包含视频列表）
// ============================================================================

/**
 * @brief 创建影视页面
 * @return QWidget* 影视页面对象
 */
QWidget* MainWindow::createMoviePage()
{
    // 进入影视界面,将视频音频频道跳到不使用设备的频道
    AudioDecode::m_Wichchannel = 0;
    VideoDecode::m_WichChannel = 0;

    QWidget* page = new QWidget();
    page->setStyleSheet("background-color: #141A1A;");

    QVBoxLayout* mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(15);

    // ========== 页面标题栏 ==========
    QWidget* titleBar = new QWidget();
    titleBar->setFixedHeight(45);
    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);

    QLabel* titleLabel = new QLabel("🎬 本地视频");
    titleLabel->setStyleSheet(
        "color: #00d4ff; "
        "font-size: 22px; "
        "font-weight: bold; "
        "background: transparent;"
    );

    // 视频数量标签
    m_movieCountLabel = new QLabel("共 0 个视频");
    m_movieCountLabel->setStyleSheet(
        "color: #888888; "
        "font-size: 13px; "
        "background: transparent;"
    );

    // 添加视频按钮
    QPushButton* addBtn = new QPushButton("+ 添加视频");
    addBtn->setFixedSize(100, 32);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #00d4ff; "
        "  color: #141A1A; "
        "  border: none; "
        "  border-radius: 16px; "
        "  font-size: 13px; "
        "  font-weight: bold; "
        "}"
        "QPushButton:hover { background-color: #33ddff; }"
        "QPushButton:pressed { background-color: #00b8e6; }"
    );

    // 刷新按钮
    QPushButton* refreshBtn = new QPushButton("刷新");
    refreshBtn->setFixedSize(70, 32);
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: rgba(255,255,255,0.1); "
        "  color: #cccccc; "
        "  border: 1px solid #333333; "
        "  border-radius: 16px; "
        "  font-size: 13px; "
        "}"
        "QPushButton:hover { "
        "  background-color: rgba(255,255,255,0.2); "
        "  color: #ffffff; "
        "}"
    );

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(m_movieCountLabel);
    titleLayout->addSpacing(15);
    titleLayout->addWidget(refreshBtn);
    titleLayout->addSpacing(10);
    titleLayout->addWidget(addBtn);

    // ========== 视频滚动区域 ==========
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true); //让视频卡片自适应窗口变化
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollArea > QWidget > QWidget { background-color: transparent; }"
    );
    // 强制隐藏横向滚动条，只保留纵向
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // 为纵向滚动条注入高级暗黑系样式表
     scrollArea->verticalScrollBar()->setStyleSheet(
         "QScrollBar:vertical {"
         "    background: transparent;"   // 轨道背景透明
         "    width: 8px;"                // 细长型，更美观
         "    margin: 0px 0px 0px 0px;"
         "}"
         "QScrollBar::handle:vertical {"
         "    background: #3A303A;"       // 滑块颜色
         "    min-height: 30px;"          // 滑块最小高度
         "    border-radius: 4px;"        // 圆角
         "}"
         "QScrollBar::handle:vertical:hover {"
         "    background: #00ADB5;"       // 鼠标悬浮时变成主题色亮青色
         "}"
         "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
         "    height: 0px;"               // 隐藏上下难看的箭头按钮
         "}"
         "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
         "    background: none;"          // 隐藏轨道背景阴影
         "}"
     );
    // 视频容器（用于放置视频卡片）
    m_movieContainer = new QWidget();
    m_movieContainer->setStyleSheet("background-color: transparent;");

    m_movieGridLayout = new QGridLayout(m_movieContainer); // 使用网格布局
    m_movieGridLayout->setSpacing(18);
    m_movieGridLayout->setContentsMargins(10, 10, 10, 10);
    m_movieGridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);// 顶部以及左对齐

    scrollArea->setWidget(m_movieContainer);

    // ========== 添加到布局 ==========
    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(scrollArea, 1);// 添加1点距离

    // ========== 连接信号 ==========
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddMovieClicked);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshMovieClicked);

    return page;
}

// ============================================================================
//   加载影视页面视频列表
// ============================================================================

void MainWindow::loadMovieVideos()
{
    // 如果视频区域的容器未空,则返回
    if(!m_movieContainer || !m_movieGridLayout)
    {
        return;
    }

    // 清空现有视频卡片
    clearMovieGrid();

    // 获取视频列表
    VideoController* controller = VideoController::getInstance();
    QList<VideoInfo> videos = controller->getAllVideos();

    // 更新数量标签
    if(m_movieCountLabel)
    {
        m_movieCountLabel->setText(QString("共 %1 个视频").arg(videos.size()));
    }

    // 检查是否有视频
    if(videos.isEmpty())
    {
        showEmptyMovieHint();
        return;
    }

    // 每行显示3个视频
    int columnCount = 3;

    for(int i = 0; i < videos.size(); i++)
    {
        const VideoInfo& info = videos[i];

        // 创建视频卡片
        VideoCardWidget* card = new VideoCardWidget(info, m_movieContainer);


        // 连接点击信号
        connect(card, &VideoCardWidget::clicked, this, &MainWindow::onMovieCardClicked);
        connect(card, &VideoCardWidget::deleteRequested, this, &MainWindow::onMovieCardDeleteRequested);

        // 计算行列位置
        int row = i / columnCount;
        int col = i % columnCount;

        // 添加到网格布局
        m_movieGridLayout->addWidget(card, row, col);
    }

    qDebug() << "[成功] 影视页面加载了" << videos.size() << "个视频";
}

// ============================================================================
//   清空影视网格
// ============================================================================

void MainWindow::clearMovieGrid()
{
    if(!m_movieGridLayout)
    {
        return;
    }

    QLayoutItem* item;
    // takeAt(0)拿走视频卡片 从第0个开始拿(第一个开始拿跟数组一样[0])
    while((item = m_movieGridLayout->takeAt(0)) != nullptr) // takeAt(0)：只是把控件从布局里挪走，控件还在内存里
    {
        if(item->widget())
        {
            delete item->widget(); //真正销毁视频缩略图控件，释放内存。
        }
        delete item;
    }
}

// ============================================================================
// 显示空提示
// ============================================================================

void MainWindow::showEmptyMovieHint()
{
    QWidget* hintWidget = new QWidget();
    hintWidget->setMinimumHeight(350);

    QVBoxLayout* hintLayout = new QVBoxLayout(hintWidget);
    hintLayout->setAlignment(Qt::AlignCenter);

    QLabel* iconLabel = new QLabel("🎬");
    iconLabel->setStyleSheet("font-size: 48px; background: transparent;");
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* textLabel = new QLabel("暂无本地视频\n点击\"添加视频\"导入");
    textLabel->setStyleSheet(
        "color: #666666; "
        "font-size: 16px; "
        "background: transparent; "
        "line-height: 24px;"
    );
    textLabel->setAlignment(Qt::AlignCenter);

    hintLayout->addWidget(iconLabel);
    hintLayout->addSpacing(10);
    hintLayout->addWidget(textLabel);

    m_movieGridLayout->addWidget(hintWidget, 0, 0, 1, 4);
}

// ============================================================================
// 槽函数：添加视频
// ============================================================================

void MainWindow::onAddMovieClicked()
{
    QString videoPath = QFileDialog::getOpenFileName(
        this,
        "选择视频文件",
        "",
        "视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv *.flv);;所有文件 (*.*)"
    );

    if(videoPath.isEmpty())
    {
        return;
    }

    QString coverPath = QFileDialog::getOpenFileName(
        this,
        "选择封面图片（可选）",
        "",
        "图片文件 (*.jpg *.jpeg *.png *.bmp);;所有文件 (*.*)"
    );

    VideoController* controller = VideoController::getInstance();

    if(controller->addVideo(videoPath, coverPath))
    {
        QMessageBox::information(this, "成功", "视频添加成功！");
        loadMovieVideos();
    }
    else
    {
        QMessageBox::warning(this, "失败", "视频添加失败，可能已存在！");
    }
}

// ============================================================================
// 槽函数：刷新视频列表
// ============================================================================

void MainWindow::onRefreshMovieClicked()
{
    loadMovieVideos();
}
// ============================================================================
// 打开播放窗口
// ============================================================================

void MainWindow::openPlayWindow(const VideoInfo& info)
{
    if(m_playWindow)
    {
        m_playWindow->close();
        m_playWindow = nullptr;
    }

    m_playWindow = new PlayWindow(info.videoPath, info.videoName);

    connect(m_playWindow, &PlayWindow::destroyed, this, &MainWindow::onPlayWindowClosed);

    m_playWindow->show();

    qDebug() << "[播放] 打开播放窗口:" << info.videoName;
}

// ============================================================================
//  播放窗口关闭回调
// ============================================================================

void MainWindow::onPlayWindowClosed()
{
    m_playWindow = nullptr;
    qDebug() << "[播放] 播放窗口已关闭";
}
// ============================================================================
// 槽函数：视频卡片点击
// ============================================================================

void MainWindow::onMovieCardClicked(int videoId)
{
    VideoController* controller = VideoController::getInstance();
    VideoInfo info;

    if(controller->getVideoById(videoId, info))
    {
        openPlayWindow(info);
    }
    else
    {
        QMessageBox::warning(this, "错误", "无法获取视频信息");
    }
}

// ============================================================================
// 槽函数：删除视频
// ============================================================================

void MainWindow::onMovieCardDeleteRequested(int videoId)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除这个视频吗？\n\n（仅删除记录，不删除本地文件）",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if(reply == QMessageBox::Yes)
    {
        VideoController* controller = VideoController::getInstance();

        if(controller->deleteVideo(videoId))
        {

            loadMovieVideos();
        }
        else
        {
            QMessageBox::warning(this, "失败", "视频删除失败！");
        }
    }
}

// ============================================================================
// 创建个人中心页面（账户信息、修改名称、退出登录等）
// ============================================================================
QWidget* MainWindow::createAccountPage()
{
    // 调整为非FFmpeg频道
    AudioDecode::m_Wichchannel = -1;
    VideoDecode::m_WichChannel = -1;

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #191419;");

    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 中间的卡片容器
    QFrame *cardFrame = new QFrame(page);
    cardFrame->setFixedSize(400, 480);
    cardFrame->setStyleSheet(
        "QFrame {"
        "   background-color: #241D24;"
        "   border-radius: 20px;"
        "   border: 1px solid #3A303A;"
        "}"
    );

    // 为卡片添加阴影
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(page);
    shadow->setOffset(0, 8);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 150));
    cardFrame->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setContentsMargins(40, 50, 40, 40);
    cardLayout->setSpacing(20);
    cardLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    //  头像区
    QLabel *avatarLabel = new QLabel("🧑‍🚀", cardFrame);
    avatarLabel->setFixedSize(100, 100);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet("background-color: #3A303A; border-radius: 50px; font-size: 55px; border: none;");
    cardLayout->addWidget(avatarLabel, 0, Qt::AlignHCenter);

    // 初始化时获取用户信息
    QString userName = UserController::getInstance()->getCurrentUserName();
    int userId = UserController::getInstance()->getCurrentUserId();
    QString account = UserController::getInstance()->getCurrentAccount();
    if (userName.isEmpty()) userName = "访客用户";
    if (account.isEmpty()) account = "---";

    // 用户名及修改按钮（水平布局）
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->setAlignment(Qt::AlignCenter);

    QLabel *nameLabel = new QLabel(userName, cardFrame);
    nameLabel->setObjectName("AccountNameLabel"); // 设置对象名，方便外面动态刷新
    nameLabel->setStyleSheet("color: #FFFFFF; font-size: 26px; font-weight: bold; background: transparent; border: none;");
    nameLabel->setAlignment(Qt::AlignCenter);

    QPushButton *editNameBtn = new QPushButton("✏️", cardFrame);
    editNameBtn->setFixedSize(30, 30);
    editNameBtn->setCursor(Qt::PointingHandCursor);
    editNameBtn->setToolTip("修改用户名称");
    editNameBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; font-size: 16px; }"
        "QPushButton:hover { background-color: #3A303A; border-radius: 15px; }"
    );

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(editNameBtn);
    cardLayout->addLayout(nameLayout);

    // 账号及 ID 信息
    QLabel *accountLabel = new QLabel(QString("用户 ID: %1   |   账号: %2").arg(userId).arg(account), cardFrame);
    accountLabel->setObjectName("AccountIdLabel"); // 【关键】设置对象名，方便外面动态刷新
    accountLabel->setStyleSheet("color: #A0A0A0; font-size: 14px; background: transparent; border: none;");
    accountLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(accountLabel);

    cardLayout->addSpacing(40); // 占位间距

    // ================= 修改用户名逻辑 =================
    connect(editNameBtn, &QPushButton::clicked, this, [=]()
    {
        QString currentAccount = UserController::getInstance()->getCurrentAccount();
        //!!! 这里需要添加新的功能 :在登录界面以访客的形式登录
        if(currentAccount.isEmpty() || currentAccount == "---")
        {
            QMessageBox::warning(this, "提示", "未获取到有效账号，请重新登录！");
            return;
        }

        bool ok;
        QString newName = QInputDialog::getText(this, "修改用户名称",
                                                "请输入新的用户名称:", QLineEdit::Normal,
                                                nameLabel->text(), &ok);
        if (ok && !newName.isEmpty() && newName != nameLabel->text())
        {
            bool success = UserController::getInstance()->handleChangeUserName(newName, currentAccount);
            if(success)
            {
                nameLabel->setText(newName); // 界面更新
                QMessageBox::information(this, "成功", "用户名称修改成功！");
            }
            else
            {
                QMessageBox::warning(this, "失败", "修改失败，请检查网络或数据库连接！");
            }
        }
    });

    // 退出登录按钮
    QPushButton *logoutBtn = new QPushButton("退 出 登 录", cardFrame);
    logoutBtn->setFixedSize(220, 45);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton { background-color: #E53935; color: white; border-radius: 22px; font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #EF5350; }"
        "QPushButton:pressed { background-color: #C62828; }"
    );
    cardLayout->addWidget(logoutBtn, 0, Qt::AlignHCenter);

    // 退出登录的信号槽
    connect(logoutBtn, &QPushButton::clicked, this, [=](){
        QMessageBox::StandardButton reply = QMessageBox::question(this, "退出账号", "确定要退出并返回登录界面吗？", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            UserController::getInstance()->handleLogout();
            // 退出程序
            qApp->quit();
        }
    });
    mainLayout->addWidget(cardFrame);
    return page;
}

// ============================================================================
// 创建系统设置页面
// ============================================================================
QWidget* MainWindow::createSettingsPage()
{
    AudioDecode::m_Wichchannel = -1;
    VideoDecode::m_WichChannel = -1;

    QWidget *page = new QWidget();
    page->setStyleSheet("background-color: #141914;");

    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 设置卡片容器
    QFrame *cardFrame = new QFrame(page);
    cardFrame->setFixedSize(500, 550);
    cardFrame->setStyleSheet(
        "QFrame {"
        "   background-color: #1D241D;"
        "   border-radius: 15px;"
        "   border: 1px solid #2D382D;"
        "}"
        "QLabel {"
        "   color: #FFFFFF;"
        "   font-size: 15px;"
        "   background: transparent;"
        "   border: none;"
        "}"
        "QCheckBox {"
        "   color: #E0E0E0;"
        "   font-size: 15px;"
        "   background: transparent;"
        "}"
        "QCheckBox::indicator {"
        "   width: 20px;"
        "   height: 20px;"
        "   border: 2px solid #5A6A5A;"
        "   border-radius: 4px;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: #4CAF50;"
        "   border-color: #4CAF50;"
        "}"
    );

    QVBoxLayout *cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(25);

    // 标题
    QLabel *titleLabel = new QLabel("⚙️  系统首选项", cardFrame);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; border-bottom: 1px solid #3A483A; padding-bottom: 15px;");
    cardLayout->addWidget(titleLabel);

    // 常规设置区域
    QCheckBox *autoStartCb = new QCheckBox("开机自动启动 StreamBox", cardFrame);
    QCheckBox *minToTrayCb = new QCheckBox("点击关闭按钮时最小化到托盘", cardFrame);
    minToTrayCb->setChecked(true); // 默认勾选
    cardLayout->addWidget(autoStartCb);
    cardLayout->addWidget(minToTrayCb);

    // 播放设置区域
    QLabel *playSettingLabel = new QLabel("▶ 播放设置", cardFrame);
    playSettingLabel->setStyleSheet("font-weight: bold; margin-top: 10px; color: #8BC34A;");
    cardLayout->addWidget(playSettingLabel);

    QCheckBox *hardwareDecodeCb = new QCheckBox("启用 GPU 硬件加速解码", cardFrame);
    hardwareDecodeCb->setChecked(true);
    QCheckBox *autoPlayNextCb = new QCheckBox("自动播放下一个视频", cardFrame);
    autoPlayNextCb->setChecked(true);
    cardLayout->addWidget(hardwareDecodeCb);
    cardLayout->addWidget(autoPlayNextCb);

    // 缓存管理
    QHBoxLayout *cacheLayout = new QHBoxLayout();
    QLabel *cacheLabel = new QLabel("本地缓存占用: 156.4 MB", cardFrame);
    cacheLabel->setStyleSheet("color: #B0B0B0;");

    QPushButton *clearCacheBtn = new QPushButton("清理缓存", cardFrame);
    clearCacheBtn->setFixedSize(100, 35);
    clearCacheBtn->setCursor(Qt::PointingHandCursor);
    clearCacheBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #2E7D32;" // 绿色
        "   color: white;"
        "   border-radius: 8px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #388E3C; }"
        "QPushButton:pressed { background-color: #1B5E20; }"
    );
    // 清理缓存动画模拟
    connect(clearCacheBtn, &QPushButton::clicked, this, [=](){
        cacheLabel->setText("本地缓存占用: 0.0 MB");
        QMessageBox::information(this, "清理成功", "本地图片及视频缓存已释放完毕！");
    });

    cacheLayout->addWidget(cacheLabel);
    cacheLayout->addStretch();
    cacheLayout->addWidget(clearCacheBtn);
    cardLayout->addLayout(cacheLayout);

    cardLayout->addStretch(); // 将上面的元素顶上去

    // 底部版权信息
    QLabel *aboutLabel = new QLabel("StreamBox 版本 v1.0.0 \nCopyright © 2026 StreamBox Inc.", cardFrame);
    aboutLabel->setStyleSheet("color: #606060; font-size: 12px; text-align: center;");
    aboutLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(aboutLabel);

    mainLayout->addWidget(cardFrame);
    return page;
}


// 全局样式表
void MainWindow::initStyle() {
    QString qss = R"(
        * { font-family: "Microsoft YaHei"; }

        /* 主背景带圆角 */
        #MainBg {
            background-color: #1E1E24;
            border-radius: 12px;
        }

        /* 侧边栏 */
        #LeftSidebar {
            background-color: #1E1E24;
            border-right: 1px solid #2D2D3B;
            border-top-left-radius: 12px;
            border-bottom-left-radius: 12px;
        }

        #LogoLabel {
            color: #00ADB5;
            font-size: 24px;
            font-weight: 900;

        }

        /* 侧边导航按钮 (基础样式，偏移由C++控制) */
        HoverNavButton {
            background-color: transparent;
            color: #9090A0;
            font-size: 15px;
            font-weight: bold;
            border: none;
            border-left: 4px solid transparent; /* 预留给选中条 */
            height: 50px;
        }
        HoverNavButton:hover {
            color: #FFFFFF;
        }
        HoverNavButton:checked {
            color: #00ADB5;
            background-color: #252530;
            border-left: 4px solid #00ADB5; /* 选中时左侧显示青色条 */
        }

        /* 顶部栏 */
        #TopBar {
            background-color: #252530;
            border-top-right-radius: 12px;
            border-bottom: 1px solid #2D2D3B;
        }

        /* 顶部导航按钮 */
        #TopNavBtn {
            background: transparent;
            color: #9090A0;
            font-size: 16px;
            font-weight: bold;
            border: none;
            padding: 5px 15px;
        }
        #TopNavBtn:hover { color: #FFFFFF; }
        #TopNavBtn:checked {
            color: #00ADB5;
            border-bottom: 3px solid #00ADB5;
        }

        /* 动画搜索框 */
        AnimatedSearchBox {
            background-color: #141419;
            color: #FFFFFF;
            border: 1px solid #3D3D4D;
            border-radius: 18px; /* 高度的半圆角 */
            padding: 5px 15px;
            font-size: 13px;
        }
        AnimatedSearchBox:focus {
            border: 1px solid #00ADB5;
            background-color: #1A1A20;
        }

        /* 关闭按钮 */
        #CloseBtn {
            background: transparent;
            color: #808090;
            font-size: 20px;
            font-weight: bold;
            border: none;
        }
        #CloseBtn:hover {
            color: #FF4757;
            background-color: #353542;
            border-radius: 15px;
        }
    )";
    this->setStyleSheet(qss);
}

// ============ 无边框窗口拖拽实现 ================
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 限制只能点顶部栏拖动
    if (event->button() == Qt::LeftButton && event->pos().y() < 70)
    {
        m_isDragging = true;
        m_dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        event->accept();
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_isDragging)
    {
        this->move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    event->accept();
}

// ==================淡入显示方法====================
void MainWindow::showWithAnimation()
{
    this->setWindowOpacity(0.0);
    this->show();

    QPropertyAnimation *fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(600);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutQuad);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
}
