#ifndef CUSTOM_WIDGETS_H
#define CUSTOM_WIDGETS_H

#include <QPushButton>
#include <QLineEdit>
#include <QPropertyAnimation>

// --- 动画导航按钮 (鼠标移入时文字/图标向右微移) ---
class HoverNavButton : public QPushButton
{
    Q_OBJECT
    //注册一个「能被 Qt 动画识别」的动态属性，专门用来实现鼠标移入文字平滑偏移的动画效果
    Q_PROPERTY(int textOffset READ textOffset WRITE setTextOffset)
public:
    explicit HoverNavButton(const QString &text, QWidget *parent = nullptr);
    int textOffset() const { return m_textOffset; }
    void setTextOffset(int offset);

protected:
    void enterEvent(QEvent *event) override; // 鼠标移入
    void leaveEvent(QEvent *event) override; // 鼠标移出

private:
    int m_textOffset;
    QPropertyAnimation *m_anim;
};

// --- 弹性动画搜索框 (聚焦时自动伸长) ---
class AnimatedSearchBox : public QLineEdit
{
    Q_OBJECT
public:
    explicit AnimatedSearchBox(QWidget *parent = nullptr);

protected:
    void enterEvent(QEvent *event) override;  // 获得焦点
    void leaveEvent(QEvent *event) override; // 失去焦点

private:
    QPropertyAnimation *m_widthAnim;
};

#endif // CUSTOM_WIDGETS_H
