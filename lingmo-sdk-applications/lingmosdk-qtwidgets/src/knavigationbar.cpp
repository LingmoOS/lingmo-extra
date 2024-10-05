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

#include "knavigationbar.h"
#include <QListView>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QScrollBar>
#include <QGSettings>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QLinearGradient>
#include <QDebug>
#include <QToolTip>
#include "themeController.h"
#include "parmscontroller.h"

namespace kdk
{

enum ItemType
{
    StandardItem = 0,
    SubItem,
    TagItem
};

class Delegate:public QStyledItemDelegate,public ThemeController
{
public:
    Delegate(QObject*parent,QListView*view);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QListView *m_listView;
};

class ListView : public QListView
{
public:
    ListView(QWidget*parent);

protected:
    void mousePressEvent(QMouseEvent* event);
};

class KNavigationBarPrivate:public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(KNavigationBar)

public:
    KNavigationBarPrivate(KNavigationBar*parent);
    void changeTheme();
private:
    KNavigationBar* q_ptr;
    ListView* m_pView;
    QStandardItemModel* m_pModel;
    Delegate* m_pDelegate;
};


KNavigationBar::KNavigationBar(QWidget* parent)
    :QScrollArea(parent),
      d_ptr(new KNavigationBarPrivate(this))
{
    Q_D(KNavigationBar);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    d->m_pView = new ListView (this);
    d->m_pModel = new QStandardItemModel(d->m_pView);
    d->m_pView->setModel(d->m_pModel);
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setSizeConstraint(QLayout::SizeConstraint::SetMaximumSize);
    vLayout->setContentsMargins(0,0,0,0);
    vLayout->setSpacing(0);
    vLayout->addWidget(d->m_pView);
    d->m_pView->setFocus();
    QPalette p = this->palette();
    QColor color(0,0,0,0);
    p.setColor(QPalette::Base,color);
    d->m_pView->setPalette(p);
    this->setPalette(p);
    d->m_pView->setFrameStyle(0);
    d->m_pDelegate = new Delegate(this,d->m_pView);
    d->m_pView->setItemDelegate(d->m_pDelegate);
    d->m_pView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setFrameStyle(0);
    this->setBackgroundRole(QPalette::Base);
    this->setAutoFillBackground(true);
    d->changeTheme();
    connect(d->m_pDelegate->m_gsetting,&QGSettings::changed,this,[=](){d->changeTheme();});
    //this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(Parmscontroller::self(),&Parmscontroller::modeChanged,this,[=](){
//        if(Parmscontroller::isTabletMode()) //解决导航栏滑动条切换主题为白条
//         d->m_pView->setStyleSheet("QListView item {height : 48}");
//        else
//         d->m_pView->setStyleSheet("QListView item {height : 36}");
        updateGeometry();
    });
}

void KNavigationBar::addItem(QStandardItem *item)
{
    Q_D(KNavigationBar);
    item->setData(ItemType::StandardItem,Qt::UserRole);
    d->m_pModel->appendRow(item);
}

void KNavigationBar::addSubItem(QStandardItem *subItem)
{
    Q_D(KNavigationBar);
    subItem->setData(ItemType::SubItem,Qt::UserRole);
    QPixmap pix(24,24);
    pix.fill(Qt::transparent);
    QIcon icon(pix);
    subItem->setIcon(icon);
    d->m_pModel->appendRow(subItem);
}


void KNavigationBar::addGroupItems(QList<QStandardItem *> items, const QString &tag)
{
    Q_D(KNavigationBar);
    QStandardItem *item = new QStandardItem(tag);
    item->setEnabled(false);
    item->setData(ItemType::TagItem,Qt::UserRole);
    d->m_pModel->appendRow(item);
    for (auto item : items)
    {
        item->setData(ItemType::StandardItem,Qt::UserRole);
        d->m_pModel->appendRow(item);
    }
}

void KNavigationBar::addTag(const QString &tag)
{
    Q_D(KNavigationBar);
    QStandardItem *item = new QStandardItem(tag);
    item->setEnabled(false);
    item->setData(ItemType::TagItem,Qt::UserRole);
    d->m_pModel->appendRow(item);
}

QStandardItemModel *KNavigationBar::model()
{
    Q_D(KNavigationBar);
    return d->m_pModel;
}

QListView *KNavigationBar::listview()
{
    Q_D(KNavigationBar);
    return d->m_pView;
}

KNavigationBarPrivate::KNavigationBarPrivate(KNavigationBar *parent)
    :q_ptr(parent)
{
    setParent(parent);
}

void KNavigationBarPrivate::changeTheme()
{
    Q_Q(KNavigationBar);
    m_pDelegate->initThemeStyle();
}

Delegate::Delegate(QObject *parent, QListView *view)
    :QStyledItemDelegate(parent),
      m_listView(view)
{
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLinearGradient m_linearGradient;
    QRectF rect;
    rect.setX(option.rect.x());
    rect.setY(option.rect.y()+1);
    rect.setWidth(option.rect.width());
//    rect.setHeight(option.rect.height()-2);
    rect.setHeight(option.rect.height()-Parmscontroller::parm(Parmscontroller::Parm::PM_NavigationBatInterval));

    //QPainterPath画圆角矩形
    const qreal radius = 6; //圆角半径6px
    QPainterPath path;
    path.moveTo(rect.topRight() - QPointF(radius, 0));
    path.lineTo(rect.topLeft() + QPointF(radius, 0));
    path.quadTo(rect.topLeft(), rect.topLeft() + QPointF(0, radius));
    path.lineTo(rect.bottomLeft() + QPointF(0, -radius));
    path.quadTo(rect.bottomLeft(), rect.bottomLeft() + QPointF(radius, 0));
    path.lineTo(rect.bottomRight() - QPointF(radius, 0));
    path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
    path.lineTo(rect.topRight() + QPointF(0, radius));
    path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

    int flag = index.model()->data(index,Qt::UserRole).toInt();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    QColor color;
    m_linearGradient = QLinearGradient(rect.width()/2,rect.y(),rect.width()/2,rect.height()+rect.y());
    if(!(option.state & QStyle::State_Enabled))
    {
        color=QColor("#FFB3B3B3");
    }
    else if(((m_listView->currentIndex() == index) ||(option.state & QStyle::State_Selected)|| (option.state & QStyle::State_MouseOver))
            && flag != 2)
        {
            if((m_listView->currentIndex() == index) ||(option.state & QStyle::State_Selected))
            {
                color = option.palette.highlight().color();
//                color = option.palette.windowText().color();
//                color.setAlphaF(0.15);
                if(ThemeController::themeMode() == LightTheme)
                {
                    m_linearGradient.setColorAt(0,color);
                    m_linearGradient.setColorAt(1,color);
                }
                else
                {
                    m_linearGradient.setColorAt(0,color);
                    m_linearGradient.setColorAt(1,color);
                }
            }
            else {
                //hover时
                if(ThemeController::widgetTheme() == FashionTheme)
                {
                    if(ThemeController::themeMode() == LightTheme)
                    {
                        color = option.palette.windowText().color();
                        color.setAlphaF(0.05);

                        QColor m_color("#E6E6E6");
                        QColor startColor = mixColor(m_color,QColor(Qt::black),0.05);
                        QColor endLightColor = mixColor(m_color,QColor(Qt::black),0.2);
                        m_linearGradient.setColorAt(0,startColor);
                        m_linearGradient.setColorAt(1,endLightColor);
                    }
                    else
                    {
                        color = option.palette.windowText().color();
                        color.setAlphaF(0.05);

                        QColor color("#373737");
                        QColor startColor = mixColor(color,QColor(Qt::white),0.2);
                        QColor endLightColor = mixColor(color,QColor(Qt::white),0.05);
                        m_linearGradient.setColorAt(0,startColor);
                        m_linearGradient.setColorAt(1,endLightColor);
                    }
                }
                else
                {
                    color = option.palette.windowText().color();
                    color.setAlphaF(0.05);
                }
            }
            painter->save();
            painter->setPen(QPen(Qt::NoPen));
            if(ThemeController::widgetTheme() == FashionTheme )
            {
                painter->setBrush(m_linearGradient);
            }
            else
            {
                painter->setBrush(color);
            }
            painter->drawPath(path);
            painter->restore();
        }

    switch (flag) {
    case 0://standardItem
    {
        QRect iconRect=QRect(rect.x()+16,rect.y()+(rect.height()-16)/2,16,16); //图片大小16*16 左边距16
        auto *model =dynamic_cast<QStandardItemModel*>(const_cast<QAbstractItemModel*>(index.model()));
        auto icon = model->item(index.row())->icon();

        if(ThemeController::themeMode() == DarkTheme)
              icon = ThemeController::drawSymbolicColoredPixmap(icon.pixmap(16,16));
        if((m_listView->currentIndex() == index)|| (option.state & QStyle::State_Selected))
            icon = ThemeController::drawColoredPixmap(icon.pixmap(16,16),QColor(255,255,255));
        if(!(option.state & QStyle::State_Enabled))
            icon = ThemeController::drawColoredPixmap(icon.pixmap(16,16),QColor("#FF979797"));
        icon.paint(painter,iconRect);
        QFontMetrics fm = painter->fontMetrics();
        QString elidedText = fm.elidedText(index.model()->data(index,Qt::DisplayRole).toString(),Qt::ElideRight,rect.width()-56); //左边距+图片宽度+文本图片间距+右边距

        QString mainText = index.data(Qt::DisplayRole).toString();
        if(fm.width(mainText) > rect.width()-56)
            model->item(index.row())->setToolTip(mainText);
        painter->save();
        if((m_listView->currentIndex() == index)||(option.state & QStyle::State_Selected))
            painter->setPen(option.palette.highlightedText().color());
        QFont font;
        font.setPointSize(ThemeController::systemFontSize());
        if(!(option.state & QStyle::State_Enabled))
        {
            painter->setPen(color);
        }
        painter->setFont(font);
        if(!icon.isNull()){
            painter->drawText(QRect(iconRect.right()+8,rect.y(),
                                rect.width()-56,rect.height()),Qt::AlignVCenter,elidedText); //文本 图片间距8px
        }
        else {
            painter->drawText(QRect(rect.x()+16,rect.y(),
                                rect.width()-56,rect.height()),Qt::AlignVCenter,elidedText);
        }
        painter->restore();
        break;
    }
    case 1://subItem
    {
        QRect iconRect=QRect(rect.x()+16,rect.y()+(rect.height()-16)/2,16,16); //图片大小16*16 左边距16
        auto *model =dynamic_cast<QStandardItemModel*>(const_cast<QAbstractItemModel*>(index.model()));

        QFontMetrics fm = painter->fontMetrics();
        QString elidedText = fm.elidedText(index.model()->data(index,Qt::DisplayRole).toString(),Qt::ElideRight,rect.width()-56); //左边距+图片宽度+文本图片间距+右边距

        QString mainText = index.data(Qt::DisplayRole).toString();
        if(fm.width(mainText) > rect.width()-56)
            model->item(index.row())->setToolTip(mainText);
        painter->save();
        if(/*(option.state & QStyle::State_HasFocus) &&*/ (option.state & QStyle::State_Selected))
            painter->setPen(option.palette.highlightedText().color());
        QFont font;
        font.setPointSize(ThemeController::systemFontSize());
        if(!(option.state & QStyle::State_Enabled))
        {
            painter->setPen(color);
        }
        painter->setFont(font);
        painter->drawText(QRect(iconRect.right()+8,rect.y(),
                                rect.width()-56,rect.height()),Qt::AlignVCenter,elidedText); //文本 图片间距8px
        painter->restore();
        break;
    }
    case 2://tagItem
    {
        painter->save();
        auto *model =dynamic_cast<QStandardItemModel*>(const_cast<QAbstractItemModel*>(index.model()));
        QFontMetrics fm = painter->fontMetrics();
        QString elidedText = fm.elidedText(index.model()->data(index,Qt::DisplayRole).toString(),Qt::ElideRight,rect.width()-56); //左边距+图片宽度+文本图片间距+右边距

        QString mainText = index.data(Qt::DisplayRole).toString();
        if(fm.width(mainText) > rect.width()-56)
            model->item(index.row())->setToolTip(mainText);
        if(ThemeController::themeMode() == ThemeFlag::DarkTheme)
            painter->setPen(QColor(115,115,115));
        else
            painter->setPen(QColor(140,140,140));

        QRect textRect = option.rect;
        textRect = textRect.adjusted(16,12,0,0);
        QFont font;
        font.setPointSize(ThemeController::systemFontSize());
        painter->setFont(font);
        painter->drawText(textRect,Qt::AlignVCenter,elidedText);
        painter->restore();
        break;
    }
    default:
        break;
    }
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int flag = index.model()->data(index,Qt::UserRole).toInt();
    QSize size;
    switch (flag) {
    case 2://tagItem
        size.setHeight(Parmscontroller::parm(Parmscontroller::Parm::PM_NavigationBatHeight)+12);
        break;
    default:
        size.setHeight(Parmscontroller::parm(Parmscontroller::Parm::PM_NavigationBatHeight)+Parmscontroller::parm(Parmscontroller::Parm::PM_NavigationBatInterval));
        break;
    }
    size.setWidth(option.rect.width());
    return size;
}

ListView::ListView(QWidget *parent)
    :QListView(parent)
{

}

void ListView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() & Qt::RightButton)
        return ;
    else if(event->button() & Qt::LeftButton)
        QListView::mousePressEvent(event);
}

}

#include "knavigationbar.moc";
#include "moc_knavigationbar.cpp";
