#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QProgressBar>

class SplashScreen : public QWidget {
    Q_OBJECT
public:
    explicit SplashScreen(QWidget *parent = nullptr);
    void showWithAnimation(); // 淡入显示并开始加载

signals:
    void finished(); // 动画和加载完成信号

private slots:
    void updateProgress(); // 更新进度条

private:
    QProgressBar *m_progressBar;
    int m_progressValue;
};

#endif // SPLASHSCREEN_H
