/*
 * lingmo-sdk-qtwidgets's Library
 *
 * Copyright (C) 2023, LingmoSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Zhen Sun <sunzhen1@lingmoos.cn>
 *
 */

#include "kborderbutton.h"
#include "themeController.h"
#include <QStyleOptionButton>
#include <QPainter>
#include <QDebug>
#include <QIcon>
#include "parmscontroller.h"

namespace kdk
{
class KBorderButtonPrivate:public QObject,public ThemeController
{
    Q_DECLARE_PUBLIC(KBorderButton)
    Q_OBJECT
public:
    KBorderButtonPrivate(KBorderButton*parent):q_ptr(parent)
    ,m_hoverd(false)
      ,m_radius(6)
    {setParent(parent);}
protected:
    void changeTheme();
private:
    KBorderButton* q_ptr;
    bool m_hoverd;
    int m_radius;
};
KBorderButton::KBorderButton(QWidget* parent)
    :QPushButton(parent)
    ,d_ptr(new KBorderButtonPrivate(this))
{
    Q_D(KBorderButton);
    connect(Parmscontroller::self(),&Parmscontroller::modeChanged,this,[=](){
        updateGeometry();
    });
}

KBorderButton::~KBorderButton()
{
}

QSize KBorderButton::sizeHint() const
{
    auto size = QPushButton::sizeHint();
    size.setHeight(Parmscontroller::parm(Parmscontroller::Parm::PM_PushButtonHeight));
    return size;
}
bool KBorderButton::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(KBorderButton);
    return QPushButton::eventFilter(watched,event);
}

void KBorderButton::paintEvent(QPaintEvent *event)
{
    Q_D(KBorderButton);
    QStyleOptionButton option;
    initStyleOption(&option);
    QPainter p(this);
    QColor borderColor;
    QColor fontColor;
    int borderWidth = 1 ;
    QColor mix = option.palette.brightText().color();
    QColor highlight = option.palette.highlight().color();
    if(!option.state.testFlag(QStyle::State_Enabled))
    {
        borderColor = option.palette.color(QPalette::Disabled,QPalette::Button);
        fontColor = option.palette.color(QPalette::Disabled,QPalette::HighlightedText);
    }
    else if(option.state.testFlag(QStyle::State_MouseOver))
    {

        if(option.state.testFlag(QStyle::State_Sunken))
        {
            borderColor = ThemeController::mixColor(highlight,mix,0.2);
            fontColor = ThemeController::mixColor(highlight,mix,0.2);
        }
        else
        {
            borderColor = ThemeController::mixColor(highlight,mix,0.05);
            fontColor = ThemeController::mixColor(highlight,mix,0.05);
        }
    }
    else if(option.state.testFlag(QStyle::State_HasFocus))
    {
        borderWidth = 2;
        fontColor = option.palette.buttonText().color();
        borderColor = ThemeController::mixColor(highlight,mix,0.2);
    }
    else
    {
        fontColor = option.palette.buttonText().color();
        borderColor = option.palette.button().color();
    }
    p.setBrush(Qt::NoBrush);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    p.save();
    pen.setWidth(borderWidth);
    pen.setColor(borderColor);
    p.setPen(pen);
    p.drawRoundedRect(option.rect.adjusted(1,1,-1,-1),d->m_radius,d->m_radius);
    p.restore();
    pen.setWidth(1);
    pen.setColor(fontColor);
    p.setPen(pen);
    QPoint point;
    QRect ir = option.rect;
    uint tf = Qt::AlignVCenter;
    if (!option.icon.isNull()) {
        //Center both icon and text

        QIcon::Mode mode = option.state & QStyle::State_Enabled ? QIcon::Normal
                                                         : QIcon::Disabled;
        if (mode == QIcon::Normal && option.state & QStyle::State_HasFocus)
            mode = QIcon::Active;
        QIcon::State state = QIcon::Off;
        if (option.state & QStyle::State_On)
            state = QIcon::On;

        QPixmap pixmap = option.icon.pixmap(option.iconSize, mode, state);
        pixmap = ThemeController::drawColoredPixmap(this->icon().pixmap(iconSize()),fontColor);
        int w = pixmap.width() / pixmap.devicePixelRatio();
        int h = pixmap.height() / pixmap.devicePixelRatio();

        if (!option.text.isEmpty())
            w += option.fontMetrics.boundingRect(option.rect, tf, option.text).width() + 2;

        point = QPoint(ir.x() + ir.width() / 2 - w / 2,
                       ir.y() + ir.height() / 2 - h / 2);

        w = pixmap.width() / pixmap.devicePixelRatio();

        if (option.direction == Qt::RightToLeft)
            point.rx() += w;

        p.drawPixmap(this->style()->visualPos(option.direction, option.rect, point), pixmap);

        if (option.direction == Qt::RightToLeft)
            ir.translate(-point.x() - 2, 0);
        else
            ir.translate(point.x() + w + 4, 0);

        // left-align text if there is
        if (!option.text.isEmpty())
            tf |= Qt::AlignLeft;
    }
    else
    {
        tf |= Qt::AlignHCenter;
    }
    p.drawText(ir,tf,option.text);
}

KBorderButton::KBorderButton(const QString &text, QWidget *parent ):KBorderButton(parent)
{
    setText(text);
}

KBorderButton::KBorderButton(const QIcon &icon, const QString &text, QWidget *parent):KBorderButton(parent)
{
    setIcon(icon);
    setText(text);
}

void KBorderButton::setIcon(const QIcon &icon)
{
    Q_D(KBorderButton);
    QPushButton::setIcon(icon);
}

void KBorderButtonPrivate::changeTheme()
{
    Q_Q(KBorderButton);
    initThemeStyle();
}

}
#include "kborderbutton.moc"
#include "moc_kborderbutton.cpp"
