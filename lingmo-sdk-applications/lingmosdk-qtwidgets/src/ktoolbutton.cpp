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

#include "ktoolbutton.h"
#include "themeController.h"
#include <QTimer>
#include <QPainter>
#include <QDebug>
#include <QStyleOption>
#include <QStyleOptionToolButton>
#include <QLinearGradient>
#include "parmscontroller.h"

namespace  kdk
{
class KToolButtonPrivate:public QObject, public  ThemeController
{
    Q_DECLARE_PUBLIC(KToolButton)
    Q_OBJECT

public:
    KToolButtonPrivate(KToolButton* parent);
    ~KToolButtonPrivate(){}
    void changePalette();
    void doLoadingFlash();

protected:
    void changeTheme();

private:
    KToolButton* q_ptr;
    KToolButtonType m_type;
    QLinearGradient m_pLinearGradient;
    bool m_isLoading;
    QTimer *m_pTimer;
    int m_flashState;
    bool m_hasArrow;
    QPixmap m_arrowPixmap;
    QColor m_pixColor;
    QColor m_bkgColor;
    QColor m_clickColor;
    QColor m_focusColor;
    QColor m_hoverColor;
    QColor m_disableColor;
    QSize m_pixmapSize;
};

KToolButton::KToolButton(QWidget *parent)
    :QToolButton(parent),
      d_ptr(new KToolButtonPrivate(this))
{
    Q_D(KToolButton);
    d->m_pTimer = new QTimer(this);
    d->m_pTimer->setInterval(100);
    d->m_flashState = 0;
    d->m_isLoading = false;
    setType(Flat);
    installEventFilter(this);
    QToolButton::setIcon(QIcon::fromTheme("open-menu-symbolic"));
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    d->changeTheme();
    connect(d->m_gsetting,&QGSettings::changed,d,&KToolButtonPrivate::changeTheme);
    connect(d->m_pTimer,&QTimer::timeout,d,&KToolButtonPrivate::doLoadingFlash);
    connect(Parmscontroller::self(),&Parmscontroller::modeChanged,this,[=](){
        updateGeometry();
    });
}

KToolButtonType KToolButton::type()
{
    Q_D(KToolButton);
    return d->m_type;
}

void KToolButton::setType(KToolButtonType type)
{
    Q_D(KToolButton);
    d->m_type = type;
    d->changePalette();
}

void KToolButton::setIcon(const QIcon &icon)
{
    Q_D(KToolButton);
    QToolButton::setIcon(icon);
    d->changeTheme();
}

void KToolButton::setLoading(bool flag)
{
    Q_D(KToolButton);
    if(!isEnabled())
        return;
    if(hasArrow())
        return;
    d->m_isLoading = flag;
    if(flag)
        d->m_pTimer->start();
    else
        d->m_pTimer->stop();
}

bool KToolButton::isLoading()
{
    Q_D(KToolButton);
    return d->m_isLoading;
}

QIcon KToolButton::icon()
{
    return QToolButton::icon();
}

void KToolButton::setArrow(bool flag)
{
    Q_D(KToolButton);
    if(!d->m_isLoading)
        d->m_hasArrow = flag;
    update();
}

bool KToolButton::hasArrow() const
{
    Q_D(const KToolButton);
    return d->m_hasArrow;
}

bool KToolButton::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(KToolButton);
    QColor highlightColor = this->palette().color(QPalette::Highlight);
    if(watched == this)
    {
        //根据不同状态重绘icon颜色
        switch (event->type()) {
        case QEvent::MouseButtonPress:
            if(isEnabled()&& !d->m_isLoading)
            {
                if (ThemeController::themeMode() == LightTheme)
                    d->m_pixColor = highlightColor.darker(120);
                else
                    d->m_pixColor = highlightColor.lighter(120);
            }
            break;
        case QEvent::Enter:
            if(isEnabled()&& !d->m_isLoading)
            {
                if (ThemeController::themeMode() == LightTheme)
                    d->m_pixColor = highlightColor.darker(105);
                else
                    d->m_pixColor = highlightColor.lighter(105);
            }
            break;
        case QEvent::FocusIn:
            if(isEnabled()&& !d->m_isLoading)
            {
                if (ThemeController::themeMode()== LightTheme)
                    d->m_pixColor = highlightColor.darker(120);
                else
                    d->m_pixColor = highlightColor.lighter(120);
            }
            break;
        case QEvent::EnabledChange:
            {
            if(!isEnabled()&& !d->m_isLoading)
            {
                if(d->m_isLoading)
                {
                    d->m_isLoading = false;
                    d->m_pTimer->stop();
                }
                if (ThemeController::themeMode() == LightTheme)
                    d->m_pixColor = QColor(191,191,191);
                else
                    d->m_pixColor = QColor(95,99,104);
            }
            else if(isEnabled()&& !d->m_isLoading)
            {
                if (ThemeController::themeMode() == LightTheme)
                    d->m_pixColor = QColor(31,32,34);
                else
                    d->m_pixColor = QColor(255,255,255);
            }
            }
            break;
        case QEvent::MouseButtonRelease:
            if(isEnabled()&& !d->m_isLoading)
            {
                if (ThemeController::themeMode() == LightTheme)
                    d->m_pixColor = highlightColor.darker(105);
                else
                    d->m_pixColor = highlightColor.lighter(105);
            }
            break;
        case QEvent::Leave:
        case QEvent::FocusOut:
            if(isEnabled()&& !d->m_isLoading)
            {
                if (ThemeController::themeMode() == LightTheme)
                    d->m_pixColor = QColor(31,32,34);
                else
                    d->m_pixColor = QColor(255,255,255);
            }
            break;
        default:
            break;
        }
    }
    return QToolButton::eventFilter(watched,event);
}

QSize KToolButton::sizeHint() const
{
    Q_D(const KToolButton);
    QSize size(Parmscontroller::parm(Parmscontroller::Parm::PM_ToolButtonHeight),
            Parmscontroller::parm(Parmscontroller::Parm::PM_ToolButtonHeight));
    if(d->m_hasArrow)
        size.setWidth(Parmscontroller::parm(Parmscontroller::Parm::PM_ToolButtonHeight)+24);
    return size;
}

void KToolButton::paintEvent(QPaintEvent *event)
{
    Q_D( KToolButton);
    d->m_pLinearGradient.setStart(this->width()/2,0);
    d->m_pLinearGradient.setFinalStop(this->width()/2,this->height());
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);


    //绘制边框
    QStyleOptionToolButton option;
    initStyleOption(&option);
    d->m_arrowPixmap = QIcon::fromTheme("lingmo-down-symbolic").pixmap(option.iconSize);

    QPen pen;
    QColor color = palette().color(QPalette::Highlight);
    pen.setColor(color);
    pen.setWidth(2);
    if((QStyle::State_HasFocus & option.state) && isEnabled() && !isLoading() &&(this->focusPolicy() != Qt::FocusPolicy::NoFocus))
        painter.setPen(pen);
    else
        painter.setPen(Qt::NoPen);
    if(!isEnabled())
        painter.setBrush(d->m_disableColor);
    else if(QStyle::State_Sunken & option.state && !isLoading())
        painter.setBrush(d->m_clickColor);
    else if(QStyle::State_MouseOver & option.state && !isLoading())
        {
            if(ThemeController::widgetTheme()==FashionTheme && type() != Flat)
            {
                painter.setBrush(d->m_pLinearGradient);
            }
            else
            {
                painter.setBrush(d->m_hoverColor);
            }
        }
    else if(QStyle::State_HasFocus & option.state && !isLoading())
        painter.setBrush(d->m_focusColor);
    else
        painter.setBrush(d->m_bkgColor);
    painter.drawRoundedRect(this->rect().adjusted(1,1,-1,-1),6,6);

    //绘制图标
    QRect rect(0,0,option.iconSize.width(),option.iconSize.height());
    QPixmap pixmap = this->icon().pixmap(option.iconSize);
    if(d->m_type == KToolButtonType::Flat || !isEnabled())
        pixmap = ThemeController::drawColoredPixmap(pixmap,d->m_pixColor);
    else if(ThemeController::themeMode() == DarkTheme)
        pixmap = ThemeController::drawSymbolicColoredPixmap(pixmap);
    if(!hasArrow())
    {
        rect.moveCenter(this->rect().center());
        painter.drawPixmap(rect,pixmap);
    }
    else
    {
        QRect newRect = this->rect().adjusted(0,0,-20,0);
        rect.moveCenter(newRect.center());
        this->style()->drawItemPixmap(&painter,rect,Qt::AlignCenter,pixmap);
        QRect arrowRect(0,0,option.iconSize.width(),option.iconSize.height());
        arrowRect.moveLeft(this->rect().width()-option.iconSize.width()-8);
        arrowRect.moveTop((this->height()-option.iconSize.height())/2);
        QPixmap arrowPixmap = d->m_arrowPixmap;
        if(d->m_type == KToolButtonType::Flat || !isEnabled())
            arrowPixmap = ThemeController::drawColoredPixmap(arrowPixmap,d->m_pixColor);
        else if(ThemeController::themeMode() == DarkTheme)
            arrowPixmap = ThemeController::drawSymbolicColoredPixmap(arrowPixmap);
        painter.drawPixmap(arrowRect,arrowPixmap);
    }
}

KToolButtonPrivate::KToolButtonPrivate(KToolButton *parent)
    :q_ptr(parent),
      m_hasArrow(false)
{
      m_hoverColor = Qt::transparent;
      m_bkgColor = Qt::transparent;
      m_disableColor = Qt::transparent;
      m_clickColor = Qt::transparent;
      m_focusColor = Qt::transparent;
      setParent(parent);
}

void KToolButtonPrivate::changePalette()
{
    Q_Q(KToolButton);
    switch (this->m_type)
    {
    case Flat:
    {
        if(ThemeController::themeMode() == LightTheme)
        {
            m_bkgColor = Qt::transparent;
            m_clickColor = Qt::transparent;
            m_focusColor = Qt::transparent;
            m_hoverColor = Qt::transparent;
            m_disableColor = Qt::transparent;
        }
        else
        {
            m_bkgColor = Qt::transparent;
            m_clickColor = Qt::transparent;
            m_focusColor = Qt::transparent;
            m_hoverColor = Qt::transparent;
            m_disableColor = q->palette().color(QPalette::Disabled,QPalette::Button);
        }
        break;
    }
    case SemiFlat:
    {
        QColor baseColor = q->palette().button().color();
        QColor mix = q->palette().brightText().color();
        m_bkgColor = Qt::transparent;
        m_clickColor = mixColor(baseColor,mix,0.2);
        m_focusColor = Qt::transparent;
        if(ThemeController::widgetTheme() == FashionTheme)
            {
                if(ThemeController::themeMode() == LightTheme)
                {
                    QColor color("#E6E6E6");
                    QColor startColor = mixColor(color,QColor(Qt::black),0.05);//("#FFD9D9D9");
                    QColor endColor = mixColor(color,QColor(Qt::black),0.2);//("#FFBFBFBF");
                    m_pLinearGradient.setColorAt(0,startColor);
                    m_pLinearGradient.setColorAt(1,endColor);
                }
                else
                {
                    QColor color("#373737");
                    QColor startColor = mixColor(color,QColor(Qt::white),0.2);//("#FF5F5F5F");
                    QColor endColor = mixColor(color,QColor(Qt::white),0.05);//("#FF414141");
                    m_pLinearGradient.setColorAt(0,startColor);
                    m_pLinearGradient.setColorAt(1,endColor);
                }
            }
        else
            {
                m_hoverColor = mixColor(baseColor,mix,0.05);
            }
        m_disableColor = Qt::transparent;
        break;
    }
    case Background:
    {
        QColor baseColor = q->palette().button().color();
        QColor mix = q->palette().brightText().color();
        m_bkgColor = baseColor;
        m_clickColor = mixColor(baseColor,mix,0.2);
        m_focusColor = baseColor;
        if(ThemeController::widgetTheme()==FashionTheme)
            {
            if(ThemeController::themeMode() == LightTheme)
            {
                QColor color("#E6E6E6");
                QColor startColor = mixColor(color,QColor(Qt::black),0.05);//("#FFD9D9D9");
                QColor endColor = mixColor(color,QColor(Qt::black),0.2);//("#FFBFBFBF");
                m_pLinearGradient.setColorAt(0,startColor);
                m_pLinearGradient.setColorAt(1,endColor);
            }
            else
            {
                QColor color("#373737");
                QColor startColor = mixColor(color,QColor(Qt::white),0.2);//("#FF5F5F5F");
                QColor endColor = mixColor(color,QColor(Qt::white),0.05);//("#FF414141");
                m_pLinearGradient.setColorAt(0,startColor);
                m_pLinearGradient.setColorAt(1,endColor);
            }
            }
        else
            {
                m_hoverColor = mixColor(baseColor,mix,0.05);
            }
        m_disableColor = q->palette().color(QPalette::Disabled,QPalette::Button);
        break;
    }
    default:
        break;
    }
}

void KToolButtonPrivate::doLoadingFlash()
{
    Q_Q(KToolButton);
    if(m_flashState < 7)
        m_flashState++;
    else
        m_flashState = 0;
    if (ThemeController::themeMode() == LightTheme)
    {
        q->QToolButton::setIcon(QIcon::fromTheme(QString("lingmo-loading-%1.symbolic").arg(m_flashState)));
    }
    else
    {
        q->QToolButton::setIcon(ThemeController::drawColoredPixmap(QIcon::fromTheme(QString("lingmo-loading-%1.symbolic").arg(m_flashState)).pixmap(q->iconSize()),QColor(255,255,255)));
    }
}

void KToolButtonPrivate::changeTheme()
{
    Q_Q(KToolButton);

    initThemeStyle();
    changePalette();
    if (ThemeController::themeMode() == LightTheme)
    {
        if(q->isEnabled())
            m_pixColor = QColor(31,32,34);
        else
            m_pixColor = QColor(191,191,191);
    }
    else
    {   if(q->isEnabled())
            m_pixColor = QColor(255,255,255);
        else
            m_pixColor = QColor(95,99,104);
    }
}
}

#include "ktoolbutton.moc"
#include "moc_ktoolbutton.cpp"
