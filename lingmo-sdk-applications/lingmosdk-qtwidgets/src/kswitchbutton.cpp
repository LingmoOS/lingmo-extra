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

#include <QMouseEvent>
#include <QStyle>
#include <QStyleOption>
#include <QPainterPath>
#include "kswitchbutton.h"
#include "themeController.h"
#include <QDebug>
#include <QVariantAnimation>
#include <QtMath>

namespace kdk
{
class KSwitchButtonPrivate:public QObject,public ThemeController
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(KSwitchButton)

public:
    KSwitchButtonPrivate(KSwitchButton*parent)
        :q_ptr(parent),
         m_animation(new QVariantAnimation(parent))
    {
        setParent(parent);
        m_animation->setDuration(200);
        m_animation->setStartValue(0.0);
        m_animation->setEndValue(1.0);
        m_animation->setEasingCurve(QEasingCurve::Linear);
        m_animation->setLoopCount(1);
    }

protected:
    void changeTheme();

private:
    KSwitchButton* q_ptr;

    QVariantAnimation *m_animation;
    int m_space;                  //滑块离背景间隔
    int m_rectRadius;             //圆角角度
    int m_startX;                 //滑块开始X轴坐标
    int m_endX;                   //滑块结束X轴坐标
    QColor m_sliderColor;
    QColor m_bkgStartColor;
    QColor m_bkgEndColor;
    bool m_isHoverd;
    bool m_isTranslucent;
    QLinearGradient m_linearGradient;
    void updateColor(const QStyleOptionButton& opt);
    void drawBackground(QPainter *painter);
    void drawSlider(QPainter *painter);
    void stepChanged();
};


KSwitchButton::KSwitchButton(QWidget* parent)
    :QPushButton(parent),d_ptr(new KSwitchButtonPrivate(this))

{
    Q_D(KSwitchButton);
    d->m_isHoverd = false;
    d->m_isTranslucent = false;
    this->setCheckable(true);

    d->changeTheme();
    connect(d->m_gsetting,&QGSettings::changed, d,&KSwitchButtonPrivate::changeTheme);
    connect(d->m_animation,&QVariantAnimation::valueChanged,d,&KSwitchButtonPrivate::stepChanged);

    connect(this,&KSwitchButton::toggled,this,[=](bool flag)
    {
        d->m_endX = width()>height() ? width()-height() : 0;
        d->m_animation->start();
        Q_EMIT stateChanged(flag);
    });
    d->m_space = 4;
}

KSwitchButton::~KSwitchButton()
{

}

void KSwitchButton::setCheckable(bool flag)
{
    QPushButton::setCheckable(flag);
    this->update();
}

bool KSwitchButton::isCheckable() const
{
    return QPushButton::isCheckable();
}

bool KSwitchButton::isChecked() const
{
    return QPushButton::isChecked();
}

void KSwitchButton::setChecked(bool flag)
{
    Q_D(KSwitchButton);
    QPushButton::setChecked(flag);
    if(signalsBlocked())
    {
        d->m_animation->start();
    }
    this->update();
}

void KSwitchButton::setTranslucent(bool flag)
{
    Q_D(KSwitchButton);
    d->m_isTranslucent = flag;
}

bool KSwitchButton::isTranslucent()
{
    Q_D(KSwitchButton);
    return d->m_isTranslucent;
}

void KSwitchButton::paintEvent(QPaintEvent *event)
{
    Q_D(KSwitchButton);

    d->m_linearGradient = QLinearGradient(this->width()/2,0,this->width()/2,this->height());
    QStyleOptionButton opt;
    initStyleOption(&opt);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    d->updateColor(opt);
    d->drawBackground(&painter);
    d->drawSlider(&painter);
    painter.drawText(0, 0, width(), height(), Qt::AlignCenter, text());
}

void KSwitchButton::resizeEvent(QResizeEvent *event)
{
    Q_D(KSwitchButton);
    QPushButton::resizeEvent(event);
    if (this->isChecked())
        d->m_startX = width()>height() ? width()-height() : 0;
    else
        d->m_startX = 0;
    d->m_rectRadius = height() / 2;
    d->m_endX = width()>height() ? width()-height() : 0;
}

QSize KSwitchButton::sizeHint() const
{
    return QSize(50,24);
}

void KSwitchButtonPrivate::changeTheme()
{
    Q_Q(KSwitchButton);
    initThemeStyle();
    q->update();
}

void KSwitchButtonPrivate::updateColor(const QStyleOptionButton& opt)
{
    Q_Q(KSwitchButton);

    //根据option状态去指定以下两个子部件的颜色
    //m_sliderColor
    //m_bkgColor
    if(m_isTranslucent && !q->isChecked())//半透明效果
    {
        m_sliderColor = QColor("#FFFFFF");
        m_bkgStartColor = opt.palette.color(QPalette::BrightText);

        if(ThemeController::themeMode() == LightTheme) //浅色模式
        {
            if(!opt.state.testFlag(QStyle::State_Enabled)) //disable
            {
                m_sliderColor = opt.palette.color(QPalette::BrightText);
                m_sliderColor.setAlphaF(0.16);
                m_bkgStartColor.setAlphaF(0.1);
                return;
            }
            m_bkgStartColor.setAlphaF(0.1); //normal
            if(opt.state.testFlag(QStyle::State_MouseOver))
            {
                if (opt.state & QStyle::State_Sunken) //clicked
                {
                    m_isHoverd = false;
                    m_bkgStartColor.setAlphaF(0.21);
                }
                else //hover
                {
                    m_isHoverd = true;
                    m_bkgStartColor.setAlphaF(0.21);
                }
            }
        }
        else
        {
            if(!opt.state.testFlag(QStyle::State_Enabled)) //disable
            {
                m_sliderColor = opt.palette.color(QPalette::BrightText);
                m_sliderColor.setAlphaF(0.2);
                m_bkgStartColor.setAlphaF(0.1);
                return;
            }
            m_bkgStartColor.setAlphaF(0.1); //normal
            if(opt.state.testFlag(QStyle::State_MouseOver))
            {
                if (opt.state & QStyle::State_Sunken) //clicked
                {
                    m_isHoverd = false;
                    m_bkgStartColor.setAlphaF(0.3);
                }
                else //hover
                {
                    m_isHoverd = true;
                    m_bkgStartColor.setAlphaF(0.2);
                }
            }
        }
    }
    else //常规效果
    {
        if(!opt.state.testFlag(QStyle::State_Enabled))
        {
            m_sliderColor = opt.palette.color(QPalette::Disabled,QPalette::ButtonText);
            m_bkgStartColor = opt.palette.color(QPalette::Disabled,QPalette::Button);
            return;
        }
        QColor mix = opt.palette.brightText().color();
        m_sliderColor = QColor("#FFFFFF");
        if(q->isChecked())
            m_bkgStartColor = opt.palette.highlight().color();
        else
            m_bkgStartColor = opt.palette.button().color();
        if(opt.state.testFlag(QStyle::State_MouseOver))
        {
            if (opt.state & QStyle::State_Sunken) //clicked
            {
                m_isHoverd = false;
                m_bkgStartColor = ThemeController::mixColor(m_bkgStartColor,mix,0.2);
            }
            else //hover
            {
                m_isHoverd = true;
                if(ThemeController::themeMode() == LightTheme) //浅色模式
                {
                    if(ThemeController::widgetTheme() == FashionTheme) //和印主题
                    {
                        if(q->isChecked())//开启
                        {
                            QColor startColor = opt.palette.color(QPalette::Highlight);
                            QColor startLightColor("#E6E6E6");
                            QColor endLightColor("#000000");
                            m_bkgStartColor = ThemeController::mixColor(startColor,startLightColor,0.2);
                            m_bkgEndColor  = ThemeController::mixColor(startColor,endLightColor,0.05);

                            m_linearGradient.setColorAt(0,m_bkgStartColor);
                            m_linearGradient.setColorAt(1,m_bkgEndColor);
                        }
                        else //关闭
                        {
                            QColor startColor("#E6E6E6");
                            QColor startLightColor("#000000 ");
                            m_bkgStartColor = ThemeController::mixColor(startColor,startLightColor,0.05);
                            m_bkgEndColor  = ThemeController::mixColor(startColor,startLightColor,0.2);

                            m_linearGradient.setColorAt(0,m_bkgStartColor);
                            m_linearGradient.setColorAt(1,m_bkgEndColor);
                        }

                    }
                    else //寻光主题
                    {
                        m_bkgStartColor = ThemeController::mixColor(m_bkgStartColor,mix,0.05);
                    }

                }
                else //深色模式
                {
                    if(ThemeController::widgetTheme() == FashionTheme) //和印主题
                    {
                        if(q->isChecked())//开启
                        {
                            QColor startColor = opt.palette.color(QPalette::Highlight);
                            QColor startDarkColor("#FFFFFF");
                            m_bkgStartColor = ThemeController::mixColor(startColor,startDarkColor,0.2);
                            m_bkgEndColor  = startColor;

                            m_linearGradient.setColorAt(0,m_bkgStartColor);
                            m_linearGradient.setColorAt(1,m_bkgEndColor);
                        }
                        else //关闭
                        {
                            QColor startColor("#373737");
                            QColor startDarkColor("#FFFFFF");
                            m_bkgStartColor = ThemeController::mixColor(startColor,startDarkColor,0.2);
                            m_bkgEndColor  = ThemeController::mixColor(startColor,startDarkColor,0.05);

                            m_linearGradient.setColorAt(0,m_bkgStartColor);
                            m_linearGradient.setColorAt(1,m_bkgEndColor);
                        }

                    }
                    else //寻光
                    {
                        m_bkgStartColor = ThemeController::mixColor(m_bkgStartColor,mix,0.05); // deafult
                    }
                }

            }
        }
    }

}

void KSwitchButtonPrivate::drawBackground(QPainter *painter)
{
    Q_Q(KSwitchButton);
    painter->save();
    painter->setPen(Qt::NoPen);
    if(ThemeController::widgetTheme() == FashionTheme)
    {
        if(m_isHoverd)
        {
            m_isHoverd = false;
            if(m_isTranslucent && !q->isChecked())
                painter->setBrush(m_bkgStartColor);
            else
                painter->setBrush(m_linearGradient);
        }
        else
            painter->setBrush(m_bkgStartColor);
    }
    else
    {
        m_isHoverd = false;
        painter->setBrush(m_bkgStartColor);
    }
    QRect rect(0, 0, q->width(), q->height());

    //背景框两边是半圆，所以圆角的半径应该固定是矩形高度的一半
    int radius = rect.height() / 2;
    int circleWidth = rect.height();

    QPainterPath path;
    path.moveTo(radius, rect.top());
    path.arcTo(QRectF(rect.left(), rect.top(), circleWidth, circleWidth), 90, 180);
    path.lineTo(rect.width() - radius, rect.height());
    path.arcTo(QRectF(rect.width() - rect.height(), rect.top(), circleWidth, circleWidth), 270, 180);
    path.lineTo(radius, rect.top());
    painter->drawPath(path);
    painter->restore();
}

void KSwitchButtonPrivate::drawSlider(QPainter *painter)
{
    Q_Q(KSwitchButton);
    painter->save();
    painter->setBrush(m_sliderColor);
    painter->setPen(Qt::NoPen);
    //滑块的直径
    int sliderWidth = q->height() - m_space * 2;
    QRect sliderRect(m_startX + m_space, m_space, sliderWidth, sliderWidth);
    painter->drawEllipse(sliderRect);
    if(!q->isEnabled())
    {
        //绘制disenable状态下的小矩形条
        if(!q->isChecked())
        {
            int topX = q->width()-sliderWidth/2-10;
            int topY = (q->height()-sliderWidth/4)/2;
            QRect lineRect(topX,topY,sliderWidth/2,sliderWidth/4);
            int radius = lineRect.height()/2;
            painter->drawRoundedRect(lineRect,radius,radius);
        }
        else
        {
            int topX = 10;
            int topY = (q->height()-sliderWidth/4)/2;
            QRect lineRect(topX,topY,sliderWidth/2,sliderWidth/4);
            int radius = lineRect.height()/2;
            painter->drawRoundedRect(lineRect,radius,radius);
        }
    }
    painter->restore();
}

void KSwitchButtonPrivate::stepChanged()
{
    Q_Q(KSwitchButton);
    if(q->isChecked())
    {
        if(m_startX < m_endX)
        {
            m_startX =qCeil(m_endX * m_animation->currentValue().toDouble());
        }
        else
        {
            m_startX = m_endX;
            m_animation->stop();
        }
    }
    else
    {
        if (m_startX > 0)
        {
            double percent = 1.0 - m_animation->currentValue().toDouble();
            m_startX = qCeil(m_endX * percent);
        }
        else
        {
            m_startX = 0;
            m_animation->stop();
        }
    }
    q->update();
}

}
#include "kswitchbutton.moc"
#include "moc_kswitchbutton.cpp"

