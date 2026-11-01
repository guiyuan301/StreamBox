#ifndef VIDEOCARDWIDGET_H
#define VIDEOCARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QString>

struct VideoInfo;

/**
 * @brief 视频卡片控件 - 深色主题
 *
 * 用于在UI上显示单个视频的封面和名称
 * 支持鼠标点击事件、悬停效果、右键菜单
 */
class VideoCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoCardWidget(const VideoInfo& info, QWidget* parent = nullptr);

    int getVideoId() const { return m_videoId; }
    QString getVideoPath() const { return m_videoPath; }
    QString getVideoName() const { return m_videoName; }

    void setCover(const QPixmap& pixmap);
    void setVideoName(const QString& name);

signals:
    void clicked(int videoId);
    void deleteRequested(int videoId);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void initUI(const VideoInfo& info);
    void applyNormalStyle();
    void applyHoverStyle();

    int m_videoId;
    QString m_videoPath;
    QString m_videoName;
    QLabel* m_coverLabel;
    QLabel* m_nameLabel;
};

#endif // VIDEOCARDWIDGET_H
