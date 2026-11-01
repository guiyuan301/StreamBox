#include "custom_widgets.h"
#include <QEvent>

// ================= HoverNavButton 实现 =================
HoverNavButton::HoverNavButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent), m_textOffset(0)
{

    // 初始化动画对象
    m_anim = new QPropertyAnimation(this, "textOffset", this);
    m_anim->setDuration(200); // 200毫秒
    m_anim->setEasingCurve(QEasingCurve::OutQuad); // 平滑减速曲线 Out:结束时   Quad: 二次方函数的运动速度| Linear（默认）匀速很僵硬
    //EasingCfurve 是核心效果 动画开始时速度很快，然后逐渐平滑减速，最后缓慢停止
}

void HoverNavButton::setTextOffset(int offset)
{
    m_textOffset = offset;
    // 利用 QSS 的 padding-left 来实现文字平移效果
    QString style = QString("QPushButton { padding-left: %1px; text-align: left; }").arg(20 + offset);
    this->setStyleSheet(this->styleSheet() + style); // 追加样式
}

void HoverNavButton::enterEvent(QEvent *event)
{
    m_anim->stop();
    m_anim->setStartValue(m_textOffset);
    m_anim->setEndValue(15); // 向右偏移 15px
    m_anim->start();
    QPushButton::enterEvent(event);
}

void HoverNavButton::leaveEvent(QEvent *event)
{
    m_anim->stop();
    m_anim->setStartValue(m_textOffset);
    m_anim->setEndValue(0);  // 恢复原位
    m_anim->start();
    QPushButton::leaveEvent(event);
}

// ================= AnimatedSearchBox 实现 =================
AnimatedSearchBox::AnimatedSearchBox(QWidget *parent) : QLineEdit(parent)
{
    this->setPlaceholderText("搜索你感兴趣的内容...");
    this->setMinimumWidth(200);
    this->setMaximumWidth(200);

    m_widthAnim = new QPropertyAnimation(this, "minimumWidth", this);
    m_widthAnim->setDuration(400);
    // 使用 OutBack 曲线，会有一种超出再弹回来的高级果冻弹性效果
    m_widthAnim->setEasingCurve(QEasingCurve::OutBack);
}

void AnimatedSearchBox::enterEvent(QEvent *event)
{
    m_widthAnim->stop();
    m_widthAnim->setStartValue(this->width());
    m_widthAnim->setEndValue(350); // 聚焦时伸长到 350px
    m_widthAnim->start();
    QLineEdit::enterEvent(event);
}

void AnimatedSearchBox::leaveEvent(QEvent *event) {
    m_widthAnim->stop();
    m_widthAnim->setStartValue(this->width());
    m_widthAnim->setEndValue(200); // 失去焦点缩回 200px
    m_widthAnim->start();
    QLineEdit::leaveEvent(event);
}
