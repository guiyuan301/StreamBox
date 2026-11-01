#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "Video/audiodecode.h"
#include "Video/videodecode.h"
#include <QLabel>

/**
 * @brief 视频播放窗口 - B站风格
 *
 * 功能特性：
 * - 顶部标题栏：返回按钮 + 标题 + 最小化/最大化/关闭
 * - 中央视频区域：支持全屏切换
 * - 底部控制栏：播放/暂停 + 进度条 + 时间 + 倍速 + 音量 + 全屏
 * - 关闭时自动停止播放并释放资源
 */
class PlayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlayWindow(const QString& videoPath, const QString& videoName = "", QWidget* parent = nullptr);
    ~PlayWindow();

    void setVideoPath(const QString& videoPath);
    void setVideoName(const QString& videoName);
    //线程
    void initThread();

private slots:
    void onBackClicked();              // 返回主页面
    void onMinimizeClicked();          // 最小化
    void onMaximizeClicked();          // 最大化/还原
    void onCloseClicked();             // 关闭窗口

    void onPlayPauseClicked();         // 播放/暂停
    void onSliderPressed();            // 进度条按下
    void onSliderReleased();           // 进度条释放
    void onSliderMoved(int value);     // 进度条拖动
    void onVolumeChanged(int value);   // 音量改变
    void onSpeedChanged(int index);    // 倍速改变
    void onFullscreenClicked();        // 全屏切换

    void onVideoFrameReady(const QImage& img);   // 接收FFmpeg解码的视频帧
    void onUpdateProgress(int current, int total); // 接收FFmpeg进度更新
    void onVideoFinished();                        // 视频播放完成

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;  // 双击全屏
    void keyPressEvent(QKeyEvent* event) override;            // ESC退出全屏
    //void resizeEvent(QResizeEvent* event) override;           // 窗口大小改变
    void closeEvent(QCloseEvent* event) override;             // 关闭事件
    void mousePressEvent(QMouseEvent* event) override;        // 鼠标按下（拖拽）
    void mouseMoveEvent(QMouseEvent* event) override;         // 鼠标移动（拖拽）
    void mouseReleaseEvent(QMouseEvent* event) override;      // 鼠标释放（拖拽结束）
    void showEvent(QShowEvent* event) override ;
private:
    void initUI();
    void initConnections();
    void updatePlayPauseIcon();
    void toggleFullscreen();
    QString formatTime(qint64 milliseconds);

    // 数据成员
    QString m_videoPath;
    QString m_videoName;
    qint64 m_duration;
    bool m_isFullscreen;
    bool m_isDragging;       // 是否正在拖动进度条
    bool m_isMovingWindow;   // 是否正在拖动窗口
    QPoint m_dragStartPos;   // 窗口拖拽起始位置

    QLabel* m_videoLabel;

    // 顶部控件
    QWidget* m_titleBar;
    QPushButton* m_backBtn;
    QLabel* m_titleLabel;
    QPushButton* m_minBtn;
    QPushButton* m_maxBtn;
    QPushButton* m_closeBtn;

    // 底部控制栏
    QWidget* m_controlBar;
    QPushButton* m_playBtn;
    QLabel* m_currentTimeLabel;
    QLabel* m_totalTimeLabel;
    QSlider* m_progressSlider;
    QComboBox* m_speedCombo;
    QSlider* m_volumeSlider;
    QPushButton* m_fullscreenBtn;

    // 保存正常状态下的位置和大小（用于退出全屏）
    QRect m_normalGeometry;

    VideoDecode* decode;
    AudioDecode* audioDecode;
    bool isPaused;
};

#endif // PLAYWINDOW_H
