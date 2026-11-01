#include "playwindow.h"
#include <QUrl>
#include <QKeyEvent>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QApplication>
#include <QTimer>
#include <QShowEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QPixmap>
#include <QUrl>

// ============================================================================
// 构造函数与析构函数
// ============================================================================

PlayWindow::PlayWindow(const QString& videoPath, const QString& videoName, QWidget* parent)
    : QWidget(parent)
    , m_videoPath(videoPath)
    , m_videoName(videoName)
    , m_duration(0)
    , m_isFullscreen(false)
    , m_isDragging(false)
    , m_isMovingWindow(false)
    , m_videoLabel(nullptr)
    , decode(nullptr)
    , audioDecode(nullptr)
    , isPaused(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    initUI();
    initConnections();

    if(!m_videoPath.isEmpty())
    {
        setVideoPath(m_videoPath);
    }
}

PlayWindow::~PlayWindow()
{
    if(decode)
    {
        decode->stop();
        decode->wait(3000);
        if(decode->isRunning())
        {
            decode->terminate();
            decode->wait();
        }
        delete decode;
        decode = nullptr;
    }
    if(audioDecode)
    {
        audioDecode->stop();
        audioDecode->wait(3000);
        if(audioDecode->isRunning())
        {
            audioDecode->terminate();
            audioDecode->wait();
        }
        delete audioDecode;
        audioDecode = nullptr;
    }
}

// ============================================================================
// 初始化UI
// ============================================================================

void PlayWindow::initUI()
{
    // 步骤1：设置窗口基本属性
    setWindowTitle(m_videoName.isEmpty() ? "视频播放" : m_videoName);
    resize(1000, 650);
    setMinimumSize(800, 500);

    // 步骤2：创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    // ================================================================
    // 顶部标题栏
    // ================================================================
    m_titleBar = new QWidget();
    m_titleBar->setObjectName("titleBar");
    m_titleBar->setFixedHeight(42);
    m_titleBar->setStyleSheet(
        "#titleBar { "
        "  background-color: #18191c; "
        "}"
    );

    QHBoxLayout* titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(12, 0, 8, 0);
    titleLayout->setSpacing(12);

    // 返回按钮
    m_backBtn = new QPushButton("← 回到主界面");
    m_backBtn->setFixedSize(95, 30);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    m_backBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: transparent; "
        "  color: #999999; "
        "  border: none; "
        "  font-size: 13px; "
        "  border-radius: 6px; "
        "}"
        "QPushButton:hover { "
        "  color: #ffffff; "
        "  background-color: rgba(255,255,255,0.08); "
        "}"
    );

    // 标题
    m_titleLabel = new QLabel(m_videoName.isEmpty() ? "视频播放" : m_videoName);
    m_titleLabel->setStyleSheet(
        "color: #e0e0e0; "
        "font-size: 14px; "
        "background: transparent;"
    );

    // 右侧按钮组
    m_minBtn = new QPushButton("─");
    m_maxBtn = new QPushButton("□");
    m_closeBtn = new QPushButton("✕");

    for(auto btn : {m_minBtn, m_maxBtn, m_closeBtn})
    {
        btn->setFixedSize(40, 30);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { "
            "  background-color: transparent; "
            "  color: #999999; "
            "  border: none; "
            "  font-size: 14px; "
            "  border-radius: 6px; "
            "}"
            "QPushButton:hover { "
            "  color: #ffffff; "
            "  background-color: rgba(255,255,255,0.08); "
            "}"
        );
    }

    // 关闭按钮特殊样式（悬停时红色）
    m_closeBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: transparent; "
        "  color: #999999; "
        "  border: none; "
        "  font-size: 14px; "
        "  border-radius: 6px; "
        "}"
        "QPushButton:hover { "
        "  color: #ffffff; "
        "  background-color: #e74c3c; "
        "}"
    );

    titleLayout->addWidget(m_backBtn);
    titleLayout->addSpacing(15);
    titleLayout->addWidget(m_titleLabel, 1);
    titleLayout->addWidget(m_minBtn);
    titleLayout->addWidget(m_maxBtn);
    titleLayout->addWidget(m_closeBtn);

    // ================================================================
    // 视频显示区域
    // ================================================================
    m_videoLabel = new QLabel();
    m_videoLabel->setStyleSheet("background-color: #000000;");
    m_videoLabel->setAlignment(Qt::AlignCenter);

    // ================================================================
    // 底部控制栏
    // ================================================================
    m_controlBar = new QWidget();
    m_controlBar->setObjectName("controlBar");
    m_controlBar->setFixedHeight(56);
    m_controlBar->setStyleSheet(
        "#controlBar { "
        "  background-color: rgba(24,25,28,0.92); "
        "}"
    );

    QHBoxLayout* controlLayout = new QHBoxLayout(m_controlBar);
    controlLayout->setContentsMargins(16, 6, 16, 8);
    controlLayout->setSpacing(12);

    // --- 左侧：播放/暂停 + 时间 ---
    m_playBtn = new QPushButton();
    m_playBtn->setFixedSize(38, 38);
    m_playBtn->setCursor(Qt::PointingHandCursor);
    m_playBtn->setText("⏸");
    m_playBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: transparent; "
        "  color: #ffffff; "
        "  border: none; "
        "  font-size: 18px; "
        "  font-weight: bold; "
        "  border-radius: 19px; "
        "}"
        "QPushButton:hover { "
        "  background-color: rgba(255,255,255,0.1); "
        "}"
    );

    m_currentTimeLabel = new QLabel("00:00");
    m_currentTimeLabel->setStyleSheet(
        "color: #aaaaaa; "
        "font-size: 13px; "
        "font-family: 'Consolas', 'Monaco', monospace; "
        "background: transparent;"
    );

    QLabel* separatorLabel = new QLabel("/");
    separatorLabel->setStyleSheet(
        "color: #666666; "
        "font-size: 13px; "
        "background: transparent;"
    );

    m_totalTimeLabel = new QLabel("00:00");
    m_totalTimeLabel->setStyleSheet(
        "color: #666666; "
        "font-size: 13px; "
        "font-family: 'Consolas', 'Monaco', monospace; "
        "background: transparent;"
    );

    // --- 中间：进度条 ---
    m_progressSlider = new QSlider(Qt::Horizontal);
    m_progressSlider->setRange(0, 0);
    m_progressSlider->setFixedHeight(20);
    m_progressSlider->setCursor(Qt::PointingHandCursor);
    m_progressSlider->setStyleSheet(
        "QSlider::groove:horizontal { "
        "  height: 4px; "
        "  background-color: rgba(255,255,255,0.15); "
        "  border-radius: 2px; "
        "}"
        "QSlider::sub-page:horizontal { "
        "  height: 4px; "
        "  background-color: #00d4ff; "
        "  border-radius: 2px; "
        "}"
        "QSlider::handle:horizontal { "
        "  width: 14px; "
        "  margin: -5px 0; "
        "  background-color: #ffffff; "
        "  border-radius: 7px; "
        "}"
        "QSlider::handle:horizontal:hover { "
        "  width: 18px; "
        "  margin: -7px 0; "
        "  background-color: #00d4ff; "
        "  border-radius: 9px; "
        "}"
    );

    // --- 右侧：倍速 + 音量 + 全屏 ---
    m_speedCombo = new QComboBox();
    m_speedCombo->addItem("倍速", 1.0);
    m_speedCombo->addItem("0.5x", 0.5);
    m_speedCombo->addItem("1.0x", 1.0);
    m_speedCombo->addItem("1.25x", 1.25);
    m_speedCombo->addItem("1.5x", 1.5);
    m_speedCombo->addItem("2.0x", 2.0);
    m_speedCombo->setCurrentIndex(2);
    m_speedCombo->setFixedSize(65, 28);
    m_speedCombo->setCursor(Qt::PointingHandCursor);
    m_speedCombo->setStyleSheet(
        "QComboBox { "
        "  background-color: rgba(255,255,255,0.08); "
        "  color: #cccccc; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 2px 8px; "
        "  font-size: 12px; "
        "}"
        "QComboBox:hover { "
        "  background-color: rgba(255,255,255,0.15); "
        "}"
        "QComboBox::drop-down { "
        "  border: none; "
        "  width: 18px; "
        "}"
        "QComboBox::down-arrow { "
        "  image: none; "
        "  border-left: 5px solid transparent; "
        "  border-right: 5px solid transparent; "
        "  border-top: 6px solid #888888; "
        "  margin-right: 6px; "
        "}"
        "QComboBox QAbstractItemView { "
        "  background-color: #2d323b; "
        "  border: 1px solid #3a3f4a; "
        "  selection-background-color: #00d4ff; "
        "  selection-color: #141A1A; "
        "  outline: none; "
        "}"
        "QComboBox QAbstractItemView::item { "
        "  padding: 5px 10px; "
        "  min-height: 26px; "
        "  color: #cccccc; "
        "}"
    );

    // 音量图标 + 音量滑块
    QLabel* volumeIcon = new QLabel("🔊");
    volumeIcon->setStyleSheet("font-size: 14px; background: transparent;");

    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(80);
    m_volumeSlider->setFixedWidth(90);
    m_volumeSlider->setFixedHeight(20);
    m_volumeSlider->setCursor(Qt::PointingHandCursor);
    m_volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal { "
        "  height: 4px; "
        "  background-color: rgba(255,255,255,0.15); "
        "  border-radius: 2px; "
        "}"
        "QSlider::sub-page:horizontal { "
        "  height: 4px; "
        "  background-color: #00d4ff; "
        "  border-radius: 2px; "
        "}"
        "QSlider::handle:horizontal { "
        "  width: 12px; "
        "  margin: -4px 0; "
        "  background-color: #ffffff; "
        "  border-radius: 6px; "
        "}"
        "QSlider::handle:horizontal:hover { "
        "  background-color: #00d4ff; "
        "}"
    );

    // 全屏按钮
    m_fullscreenBtn = new QPushButton("⛶");
    m_fullscreenBtn->setFixedSize(38, 38);
    m_fullscreenBtn->setCursor(Qt::PointingHandCursor);
    m_fullscreenBtn->setToolTip("全屏 (双击也可)");
    m_fullscreenBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: transparent; "
        "  color: #cccccc; "
        "  border: none; "
        "  font-size: 16px; "
        "  border-radius: 19px; "
        "}"
        "QPushButton:hover { "
        "  color: #ffffff; "
        "  background-color: rgba(255,255,255,0.1); "
        "}"
    );

    // 组装底部控制栏
    controlLayout->addWidget(m_playBtn);
    controlLayout->addSpacing(4);
    controlLayout->addWidget(m_currentTimeLabel);
    controlLayout->addSpacing(2);
    controlLayout->addWidget(separatorLabel);
    controlLayout->addSpacing(2);
    controlLayout->addWidget(m_totalTimeLabel);
    controlLayout->addSpacing(16);
    controlLayout->addWidget(m_progressSlider, 1);
    controlLayout->addSpacing(16);
    controlLayout->addWidget(m_speedCombo);
    controlLayout->addSpacing(8);
    controlLayout->addWidget(volumeIcon);
    controlLayout->addWidget(m_volumeSlider);
    controlLayout->addSpacing(8);
    controlLayout->addWidget(m_fullscreenBtn);

    // ================================================================
    // 组装主布局
    // ================================================================
    mainLayout->addWidget(m_titleBar);
    mainLayout->addWidget(m_videoLabel, 1);
    mainLayout->addWidget(m_controlBar);

    // ================================================================
    // 创建播放器
    // ================================================================

}

//初始化线程

void PlayWindow::initThread()
{
    if(m_videoPath.isEmpty()) return;

    // ==========================================
       // 【修复1】：必须在这里设置播放模式！0代表本地影视文件模式，1代表直播模式
       // 否则 m_WichChannel 默认为 -1，会导致两个线程启动后直接退出！
       VideoDecode::m_WichChannel = 0;
       AudioDecode::m_Wichchannel = 0;
       // ==========================================
        decode = new VideoDecode();
        audioDecode = new AudioDecode();

        decode->setFilePath(m_videoPath);
        audioDecode->setFilePath(m_videoPath);

        decode->setAudioDecode(audioDecode);

        // 连接信号
        connect(decode, &VideoDecode::sendImg, this, &PlayWindow::onVideoFrameReady);
        connect(decode, &VideoDecode::updateProgress, this, &PlayWindow::onUpdateProgress);
        connect(decode, &VideoDecode::videoFinished, this, &PlayWindow::onVideoFinished);

        // 启动音频线程（先启动音频，因为视频同步依赖音频时间）
        audioDecode->start();
        // 启动视频线程
        decode->start();
}

// ============================================================================
// 初始化信号连接
// ============================================================================

void PlayWindow::initConnections()
{
    // 顶部按钮
    connect(m_backBtn, &QPushButton::clicked, this, &PlayWindow::onBackClicked);
    connect(m_minBtn, &QPushButton::clicked, this, &PlayWindow::onMinimizeClicked);
    connect(m_maxBtn, &QPushButton::clicked, this, &PlayWindow::onMaximizeClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &PlayWindow::onCloseClicked);

    // 控制栏按钮
    connect(m_playBtn, &QPushButton::clicked, this, &PlayWindow::onPlayPauseClicked);
    connect(m_fullscreenBtn, &QPushButton::clicked, this, &PlayWindow::onFullscreenClicked);

    // 进度条
    connect(m_progressSlider, &QSlider::sliderPressed, this, &PlayWindow::onSliderPressed);
    connect(m_progressSlider, &QSlider::sliderReleased, this, &PlayWindow::onSliderReleased);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &PlayWindow::onSliderMoved);

    // 音量和倍速
    connect(m_volumeSlider, &QSlider::valueChanged, this, &PlayWindow::onVolumeChanged);
    connect(m_speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PlayWindow::onSpeedChanged);

//    // 播放器信号
//    connect(decode, &VideoDecode::sendImg, this, &PlayWindow::onVideoFrameReady);
//    connect(decode, &VideoDecode::updateProgress, this, &PlayWindow::onUpdateProgress);
//    connect(decode, &VideoDecode::videoFinished, this, &PlayWindow::onVideoFinished);
}

// ============================================================================
// 设置方法
// ============================================================================

void PlayWindow::setVideoPath(const QString& videoPath)
{
    m_videoPath = videoPath;
}

void PlayWindow::setVideoName(const QString& videoName)
{
    m_videoName = videoName;
    m_titleLabel->setText(videoName);
    setWindowTitle(videoName);
}

// ============================================================================
// 顶部按钮槽函数
// ============================================================================

void PlayWindow::onBackClicked()
{
    if(decode) decode->stop();
    if(audioDecode) audioDecode->stop();
    close();
}

void PlayWindow::onMinimizeClicked()
{
    showMinimized();
}

void PlayWindow::onMaximizeClicked()
{
    if(isMaximized())
    {
        showNormal();
        m_maxBtn->setText("□");
    }
    else
    {
        showMaximized();
        m_maxBtn->setText("❐");
    }
}

void PlayWindow::onCloseClicked()
{
    close();
}

// ============================================================================
// 控制栏槽函数
// ============================================================================

void PlayWindow::onPlayPauseClicked()
{
    if(!decode) return;
    // 如果已经播放完成，点击则重新播放
    if (decode->isFinished())
    {
        decode->restart();
        if(audioDecode) audioDecode->restart();
        isPaused = false;
        updatePlayPauseIcon();
        return;
    }
    if(!isPaused)
    {
        decode->pause();
        if(audioDecode) audioDecode->pause();
        isPaused = true;
    }
    else
    {
        decode->resume();
        if(audioDecode) audioDecode->resume();
        isPaused = false;
    }
    updatePlayPauseIcon();
}

void PlayWindow::onSliderPressed()
{
    m_isDragging = true;
}

void PlayWindow::onSliderReleased()
{
    m_isDragging = false;
    int value = m_progressSlider->value();
    int seconds = value / 1000;
    if(decode) decode->seekTo(seconds);
    if(audioDecode) audioDecode->seekTo(seconds);
}

void PlayWindow::onSliderMoved(int value)
{
    // 实时更新时间显示（不跳转）
    m_currentTimeLabel->setText(formatTime(value));
}

void PlayWindow::onVolumeChanged(int value)
{
    if(audioDecode) audioDecode->setVolume(value);
}

//!当前的 VideoDecode 和 AudioDecode 没有倍速控制功能
void PlayWindow::onSpeedChanged(int index)
{
    double speed = m_speedCombo->currentData().toDouble();
    qDebug() << "[倍速] 设置为:" << speed << "x ";
}

void PlayWindow::onFullscreenClicked()
{
    toggleFullscreen();
}

// ============================================================================
// 播放器槽函数
// ============================================================================

void PlayWindow::onVideoFrameReady(const QImage& img)
{
    if(m_videoLabel)
    {
        QPixmap pixmap = QPixmap::fromImage(img.scaled(
            m_videoLabel->size(),
            Qt::KeepAspectRatio,
            Qt::FastTransformation
        ));
        m_videoLabel->setPixmap(pixmap);
    }
}

void PlayWindow::onUpdateProgress(int current, int total)
{
    m_duration = total * 1000;
    m_progressSlider->setRange(0, total * 1000);

    if(!m_isDragging)
    {
        m_progressSlider->blockSignals(true);
        m_progressSlider->setValue(current * 1000);
        m_progressSlider->blockSignals(false);

        m_currentTimeLabel->setText(formatTime(current * 1000));
    }
    m_totalTimeLabel->setText(formatTime(total * 1000));
}

void PlayWindow::onVideoFinished()
{
    isPaused = true;

    // 【修改点1】：强制将进度条和时间标签拉满
    m_progressSlider->blockSignals(true);
    m_progressSlider->setValue(m_duration);
    m_progressSlider->blockSignals(false);
    m_currentTimeLabel->setText(formatTime(m_duration));

    // 更新按钮状态为“重播”
    updatePlayPauseIcon();
    qDebug() << "[状态] 视频播放完成";

}
// ============================================================================
// 辅助方法
// ============================================================================

void PlayWindow::updatePlayPauseIcon()
{
    // ：增加视频播放结束的状态
    if (decode && decode->isFinished())
    {
        m_playBtn->setText("🔄"); // 设置为重播图标
        m_playBtn->setToolTip("重新播放");
    }
    else if (!isPaused)
    {
        m_playBtn->setText("⏸");
        m_playBtn->setToolTip("暂停 (空格键)");
    }
    else
    {
        m_playBtn->setText("▶");
        m_playBtn->setToolTip("播放 (空格键)");
    }
}

void PlayWindow::toggleFullscreen()
{
    if(m_isFullscreen)
    {
        // 退出全屏
        showNormal();
        setGeometry(m_normalGeometry);
        m_titleBar->show();
        m_controlBar->show();
        m_fullscreenBtn->setText("⛶");
        m_fullscreenBtn->setToolTip("进入全屏");
        m_isFullscreen = false;

        qDebug() << "[全屏] 已退出全屏";
    }
    else
    {
        // 进入全屏前保存当前位置和大小
        m_normalGeometry = geometry();

        showFullScreen();
        m_titleBar->hide();
        m_controlBar->hide();
        m_fullscreenBtn->setText("⛶");
        m_fullscreenBtn->setToolTip("退出全屏 (ESC或双击)");
        m_isFullscreen = true;

        qDebug() << "[全屏] 已进入全屏";
    }
}

QString PlayWindow::formatTime(qint64 milliseconds)
{
    int totalSeconds = milliseconds / 1000;
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    if(hours > 0)
    {
        return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
    else
    {
        return QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
}


// ============================================================================
// 事件处理 - 窗口显示时强制重绘并播放
// ============================================================================

void PlayWindow::closeEvent(QCloseEvent* event)
{
    if(decode)
    {
        decode->stop();
        decode->wait(3000);
        if(decode->isRunning())
        {
            decode->terminate();
            decode->wait();
        }
    }
    if(audioDecode)
    {
        audioDecode->stop();
        audioDecode->wait(3000);
        if(audioDecode->isRunning())
        {
            audioDecode->terminate();
            audioDecode->wait();
        }
    }
    event->accept();
}

void PlayWindow::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton && !m_isFullscreen)
    {
        // 只在标题栏区域允许拖拽
        if(event->y() <= m_titleBar->height())
        {
            m_isMovingWindow = true;
            m_dragStartPos = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
    QWidget::mousePressEvent(event);
}

void PlayWindow::mouseMoveEvent(QMouseEvent* event)
{
    if(m_isMovingWindow && (event->buttons() & Qt::LeftButton) && !m_isFullscreen)
    {
        move(event->globalPos() - m_dragStartPos);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void PlayWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_isMovingWindow)
    {
        m_isMovingWindow = false;
        event->accept();
    }
    QWidget::mouseReleaseEvent(event);
}

void PlayWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    if(m_videoPath.isEmpty()) return;

    qDebug() << "[显示] showEvent 触发";

    if(!decode && !audioDecode)
    {
        initThread();
    }
    else if(isPaused)
    {
        decode->resume();
        if(audioDecode) audioDecode->resume();
        isPaused = false;
        updatePlayPauseIcon();
    }
}
void PlayWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton && !m_isFullscreen)
    {
        toggleFullscreen();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void PlayWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape && m_isFullscreen)
    {
        toggleFullscreen();
    }
    else if(event->key() == Qt::Key_Space)
    {
        onPlayPauseClicked();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

