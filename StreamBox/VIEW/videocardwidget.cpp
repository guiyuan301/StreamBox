#include "videocardwidget.h"
#include "CONTROLLER/videocontroller.h"
#include <QPixmap>
#include <QMenu>
#include <QPainter>
#include <QDebug>

VideoCardWidget::VideoCardWidget(const VideoInfo& info, QWidget* parent)
    : QWidget(parent)
    , m_videoId(info.videoId)
    , m_videoPath(info.videoPath)
    , m_videoName(info.videoName)
{
    initUI(info);
    applyNormalStyle();
}

void VideoCardWidget::initUI(const VideoInfo& info)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    // 封面标签 - 圆角容器效果
    QWidget* coverContainer = new QWidget();
    coverContainer->setFixedSize(160, 220);
    coverContainer->setObjectName("coverContainer");

    QVBoxLayout* coverLayout = new QVBoxLayout(coverContainer);
    coverLayout->setContentsMargins(0, 0, 0, 0);

    m_coverLabel = new QLabel(coverContainer);
    m_coverLabel->setScaledContents(true);
    m_coverLabel->setAlignment(Qt::AlignCenter);
    m_coverLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 通过Controller加载封面
    VideoController* controller = VideoController::getInstance();
    QPixmap cover = controller->loadCover(info.coverPath);

    if(cover.isNull())
    {
        // 无封面时显示占位图标和文字
        m_coverLabel->setText("暂无封面");
        m_coverLabel->setStyleSheet(
            "QLabel { "
            "  background-color: #2a2e35; "
            "  color: #555555; "
            "  font-size: 13px; "
            "}"
        );
    }
    else
    {
        m_coverLabel->setPixmap(cover);
    }

    coverLayout->addWidget(m_coverLabel);

    // 名称标签
    m_nameLabel = new QLabel(info.videoName);
    m_nameLabel->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    m_nameLabel->setWordWrap(true);
    m_nameLabel->setFixedWidth(180);
    m_nameLabel->setFixedHeight(36);
    m_nameLabel->setStyleSheet(
        "color: #cccccc; "
        "font-size: 13px; "
        "background: transparent;"
    );

    layout->addWidget(coverContainer, 0, Qt::AlignCenter);
    layout->addWidget(m_nameLabel, 0, Qt::AlignCenter);

    this->setFixedSize(200, 290);
}

void VideoCardWidget::applyNormalStyle()
{
    this->setStyleSheet(
        "VideoCardWidget { "
        "  background-color: #1e2329; "
        "  border: 1px solid #2a2e35; "
        "  border-radius: 12px; "
        "}"
        "#coverContainer { "
        "  background-color: #252a32; "
        "  border-radius: 8px; "
        "}"
    );
}

void VideoCardWidget::applyHoverStyle()
{
    this->setStyleSheet(
        "VideoCardWidget { "
        "  background-color: #282c34; "
        "  border: 1px solid #00d4ff; "
        "  border-radius: 12px; "
        "}"
        "#coverContainer { "
        "  background-color: #2d323b; "
        "  border-radius: 8px; "
        "}"
    );
}

void VideoCardWidget::setCover(const QPixmap& pixmap)
{
    if(!pixmap.isNull())
    {
        m_coverLabel->setPixmap(pixmap);
        m_coverLabel->setStyleSheet("");
    }
}

void VideoCardWidget::setVideoName(const QString& name)
{
    m_videoName = name;
    m_nameLabel->setText(name);
}

void VideoCardWidget::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit clicked(m_videoId);
    }
    QWidget::mousePressEvent(event);
}

void VideoCardWidget::enterEvent(QEvent* event)
{
    applyHoverStyle();
    this->setCursor(Qt::PointingHandCursor);
    QWidget::enterEvent(event);
}

void VideoCardWidget::leaveEvent(QEvent* event)
{
    applyNormalStyle();
    this->unsetCursor();
    QWidget::leaveEvent(event);
}

void VideoCardWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu;
    menu.setStyleSheet(
        "QMenu { "
        "  background-color: #2d323b; "
        "  border: 1px solid #3a3f4a; "
        "  border-radius: 6px; "
        "  padding: 5px; "
        "}"
        "QMenu::item { "
        "  padding: 8px 30px 8px 20px; "
        "  border-radius: 4px; "
        "  color: #cccccc; "
        "  font-size: 13px; "
        "}"
        "QMenu::item:selected { "
        "  background-color: #00d4ff; "
        "  color: #141A1A; "
        "}"
        "QMenu::separator { "
        "  height: 1px; "
        "  background-color: #3a3f4a; "
        "  margin: 5px 10px; "
        "}"
    );

    QAction* playAction = menu.addAction("▶ 播放");
    menu.addSeparator();
    QAction* deleteAction = menu.addAction("🗑 删除视频");

    QAction* selectedAction = menu.exec(event->globalPos());

    if(selectedAction == playAction)
    {
        emit clicked(m_videoId);
    }
    else if(selectedAction == deleteAction)
    {
        emit deleteRequested(m_videoId);
    }
}
