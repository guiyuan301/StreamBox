#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStackedWidget>
#include <QButtonGroup>
#include "custom_widgets.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include "playwindow.h"
#include <QList>

class PlayWindow;
class VideoCardWidget;
struct VideoInfo;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void showWithAnimation();  //动画
signals:
    void closedSignal(); //通知程序退出
private slots:
    void onAddMovieClicked();
    void onRefreshMovieClicked();
    void onMovieCardClicked(int videoId);
    void onMovieCardDeleteRequested(int videoId);
    void onPlayWindowClosed();
protected:
    // 无边框窗口拖动 逻辑同理登录注册窗口
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initUI();       //初始化布局
    void initStyle();    //全局样式

    //创建四个界面
    QWidget* createMoviePage();
    QWidget* createAccountPage();
    QWidget* createSettingsPage();
    QWidget* createLivePage();

    void loadMovieVideos();   //加载本地视频
    void clearMovieGrid();    //清理本地视频(只清理展示出来的,不删除文件中的)
    void showEmptyMovieHint();//清理之后或者没添加视频的空画面
    QWidget* createPage(const QString &title, const QString &bgColor); // 创建子页面的快捷方法
    void openPlayWindow(const VideoInfo& info); //创建新的界面 播放本地视频界面
private:
    QStackedWidget *m_stackedWidget; // 页面堆叠容器(创建四个画面)
    QButtonGroup *m_menuGroup;       // 按钮逻辑组（用于互斥选中）

    // 拖动参数
    bool m_isDragging;
    QPoint m_dragPosition;
    // 影视页面相关控件
    QWidget* m_movieContainer;
    QGridLayout* m_movieGridLayout;
    QLabel* m_movieCountLabel;
    PlayWindow* m_playWindow;
    QList<VideoInfo> m_allVideos;
};

#endif // MAINWINDOW_H
