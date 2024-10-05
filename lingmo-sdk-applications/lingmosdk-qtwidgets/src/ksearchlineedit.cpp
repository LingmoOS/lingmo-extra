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

#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QBoxLayout>
#include <QAction>
#include <QEvent>
#include <QStringListModel>
#include <QCompleter>
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QDebug>
#include <QStyleOption>
#include <QListView>
#include <QPainter>
#include <QPalette>
#include "ksearchlineedit.h"
#include "themeController.h"
#include <QApplication>
#include <QStyle>
#include <QProxyStyle>
#include <QPainterPath>
#include <QToolTip>
#include <QCursor>
#include "kshadowhelper.h"
#include "parmscontroller.h"

namespace kdk
{

class MyStyle:public QProxyStyle
{
public:
    MyStyle(){}
    ~MyStyle(){}

    void drawControl(QStyle::ControlElement element,
                        const QStyleOption *option,
                        QPainter *painter,
                        const QWidget *widget = nullptr) const override;

    QRect subElementRect(QStyle::SubElement element,
                         const QStyleOption *option,
                         const QWidget *widget) const override;
};

class ListViewDelegate:public QStyledItemDelegate,public ThemeController
{
public:
    ListViewDelegate(QObject*parent);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

static const int iconMargin = 6;
static const int buttonMargin = 6;
static const int icontextSpacing = 5;

class KSearchLineEditPrivate:public QObject,public ThemeController
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(KSearchLineEdit)

public:
    KSearchLineEditPrivate(KSearchLineEdit*parent);
    void adjustHolderRect();

public Q_SLOTS:
    void doAnimationFinished();
    void doEditingFinished();

protected:
    void changeTheme();
private:
    KSearchLineEdit* q_ptr;
    QHBoxLayout* m_pWidgetLayout;
    QWidget* m_pWidget;
    QLabel* m_pTextLabel;
    QLabel* m_pIconLabel;
    QPropertyAnimation* m_pAnimation;
    bool m_isSearching;
    QStringListModel* m_pListModel;
    QStringList m_List;
    QCompleter* m_pCompleter;

    QString m_placeHolderText;
    Qt::Alignment m_placeHolderAlignment;
    bool m_hasFocus;
    QRect m_holderRect;
    QRect m_sideRect;
    ListViewDelegate *m_pListViewDelegate;
    bool m_isTranslucent;
    bool isTextEmpty;
    void init();

};

KSearchLineEdit::KSearchLineEdit(QWidget *parent)
    :QLineEdit(parent),
      d_ptr(new KSearchLineEditPrivate(this))
{
    Q_D(KSearchLineEdit);

    setFocusPolicy(Qt::ClickFocus);
    //应设计师要求，1.2.0.8之后的版本默认显示右键菜单
    //setContextMenuPolicy(Qt::NoContextMenu);
    d->init();
    installEventFilter(this);

    connect(d->m_pAnimation,&QPropertyAnimation::finished,d,&KSearchLineEditPrivate::doAnimationFinished);
    connect(d->m_gsetting,&QGSettings::changed,d,&KSearchLineEditPrivate::changeTheme);
    connect(this,&KSearchLineEdit::editingFinished,d,&KSearchLineEditPrivate::doEditingFinished);
    connect(this,&KSearchLineEdit::returnPressed,this,&KSearchLineEdit::clearFocus);
    connect(Parmscontroller::self(),&Parmscontroller::modeChanged,this,[=](){
        updateGeometry();
    });
    d->changeTheme();
}

KSearchLineEdit::~KSearchLineEdit()
{

}

void KSearchLineEdit::setEnabled(bool enable)
{
    Q_D(KSearchLineEdit);
    QLineEdit::setEnabled(enable);
    d->changeTheme();
}

bool KSearchLineEdit::isEnabled()
{
    Q_D(KSearchLineEdit);
    return QLineEdit::isEnabled();
}

void KSearchLineEdit::setClearButtonEnabled(bool enable)
{
    Q_D(KSearchLineEdit);
    QLineEdit::setClearButtonEnabled(enable);
}

bool KSearchLineEdit::isClearButtonEnabled() const
{
    Q_D(const KSearchLineEdit);
    QLineEdit::isClearButtonEnabled();
}

QString KSearchLineEdit::placeholderText() const
{
    Q_D(const KSearchLineEdit);
    return d->m_placeHolderText;
}

void KSearchLineEdit::setPlaceholderText(const QString &text)
{
    Q_D(KSearchLineEdit);
    d->m_placeHolderText = text;
    d->m_pTextLabel->setText(text);
}

Qt::Alignment KSearchLineEdit::placeholderAlignment() const
{
    Q_D(const KSearchLineEdit);
    return d->m_placeHolderAlignment;
}

void KSearchLineEdit::setPlaceholderAlignment(Qt::Alignment flag)
{
    Q_D(KSearchLineEdit);
    d->m_placeHolderAlignment = flag;
}

Qt::Alignment KSearchLineEdit::alignment() const
{
    return QLineEdit::alignment();
}

void KSearchLineEdit::setAlignment(Qt::Alignment flag)
{
    Q_D(KSearchLineEdit);
    QLineEdit::setAlignment(flag);
}

void KSearchLineEdit::setTranslucent(bool flag)
{
    Q_D(KSearchLineEdit);
    d->m_isTranslucent = flag;
    setProperty("needTranslucent",flag);
}

bool KSearchLineEdit::isTranslucent()
{
    Q_D(KSearchLineEdit);
    return d->m_isTranslucent;
}

void KSearchLineEdit::reloadStyle()
{
    Q_D(KSearchLineEdit);
    QAbstractItemView *popup = completer()->popup();
    kdk::effects::KShadowHelper::self()->setWidget(popup);

    popup->setItemDelegate(d->m_pListViewDelegate);
    MyStyle *style = new MyStyle;
    popup->setStyle(style);
}

void KSearchLineEdit::clear()
{
    Q_D(KSearchLineEdit);

    if(this->text().isEmpty())
        return;
    QLineEdit::clear();
    d->isTextEmpty = true;
    if(this->hasFocus())
    {
        d->m_pTextLabel->setVisible(false);
        d->m_pTextLabel->adjustSize();
        d->m_pWidget->update();
    }
    else
    {
        d->m_pTextLabel->setVisible(true);
        d->m_pTextLabel->adjustSize();
        d->adjustHolderRect();
        d->m_pAnimation->setStartValue(d->m_sideRect);
        d->m_pAnimation->setEndValue(d->m_holderRect);
        d->m_pAnimation->start();
    }
}

bool KSearchLineEdit::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(KSearchLineEdit);

    QFont font;
    font=QApplication::font();
    QFontMetrics fm(font);

    if(watched == this)
    {
        if(event->type() == QEvent::Show)
        {
            d->adjustHolderRect();
            if(text().isEmpty())
                d->m_pWidget->setGeometry(d->m_holderRect);
            else
                d->m_pWidget->setGeometry(d->m_sideRect);
        }
        if(event->type() == QEvent::FocusIn)
        {
            d->m_isSearching = true;
            d->m_hasFocus = false;
            if(!this->text().isEmpty())
            {
                if(d->isTextEmpty)
                {
                    d->m_pTextLabel->setVisible(false);
                    d->isTextEmpty=false;
                }
            }
            else
            {
                d->m_pTextLabel->setVisible(false);
                if(d->isTextEmpty)
                {
                    d->adjustHolderRect();
                    d->m_pAnimation->setStartValue(d->m_holderRect);
                    d->m_pAnimation->setEndValue(d->m_sideRect);
                    d->m_pAnimation->start();
                    d->isTextEmpty = false;
                }
            }
        }
        else if(event->type() == QEvent::FocusOut)
        {
            d->m_isSearching = false;
            d->m_hasFocus = true;
            if(!this->text().isEmpty())
            {
                d->m_pTextLabel->setVisible(false);
            }
            else
            {
                d->m_pTextLabel->setVisible(true);
                d->adjustHolderRect();
                d->m_pAnimation->setStartValue(d->m_sideRect);
                d->m_pAnimation->setEndValue(d->m_holderRect);
                d->m_pAnimation->start();
                d->isTextEmpty = true;
            }
        }
        else if(event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverMove )
        {
            if(width()-icontextSpacing-d->m_pIconLabel->width()<fm.width(d->m_placeHolderText))
            {
                if(d->m_hasFocus)
                    setToolTip(d->m_placeHolderText);
                else
                    setToolTip("");
            }
        }
    }
    return QLineEdit::eventFilter(watched,event);
}

void KSearchLineEditPrivate::adjustHolderRect()
{
    Q_Q(KSearchLineEdit);
    m_pWidget->adjustSize();
    auto maxVisualWidth = q->width()-icontextSpacing-m_pIconLabel->width();
    QFont font(QApplication::font());
    QFontMetrics fm(font);
    auto elidedText = fm.elidedText(m_placeHolderText,Qt::ElideRight,maxVisualWidth);
    m_pTextLabel->setText(elidedText);
    m_pWidget->adjustSize();


    m_sideRect = m_pWidget->rect();
    m_sideRect.moveTop((q->rect().height()-m_pWidget->height())/2);
    m_sideRect.moveLeft(iconMargin);
    if(m_placeHolderAlignment & Qt::AlignCenter)
    {
        m_holderRect = m_pWidget->rect();
        m_holderRect.moveCenter(q->rect().center());
    }
    else if(m_placeHolderAlignment & Qt::AlignLeft)
    {
        m_holderRect = m_pWidget->rect();
        m_holderRect.moveTop((q->rect().height()-m_pWidget->height())/2);
        m_holderRect.moveLeft(iconMargin);
    }

    else if(m_placeHolderAlignment & Qt::AlignRight)
    {
        m_holderRect = m_pWidget->rect();
        m_holderRect.moveTop((q->height()-m_pWidget->height())/2);
        m_holderRect.moveRight(q->width() - iconMargin);
    }

}

void KSearchLineEdit::resizeEvent(QResizeEvent *event)
{
    Q_D(KSearchLineEdit);
    QLineEdit::resizeEvent(event);
    d->adjustHolderRect();
    if(text().isEmpty() && !hasFocus())
        d->m_pWidget->setGeometry(d->m_holderRect);
    else
        d->m_pWidget->setGeometry(d->m_sideRect);
}

void KSearchLineEdit::setVisible(bool visible)
{
    Q_D(KSearchLineEdit);


    QLineEdit::setVisible(visible);
    if(!text().isEmpty())
    {
        d->m_pTextLabel->setVisible(false);
        d->adjustHolderRect();
        d->m_pWidget->setGeometry(d->m_sideRect);
    }
    else
    {
        d->m_pTextLabel->setVisible(true);
    }
}

QSize KSearchLineEdit::sizeHint() const
{
    auto size = QLineEdit::sizeHint();
    size.setHeight(Parmscontroller::parm(Parmscontroller::Parm::PM_SearchLineEditHeight));
    return size;
}

KSearchLineEditPrivate::KSearchLineEditPrivate(KSearchLineEdit *parent)
    :q_ptr(parent),
      m_placeHolderText(tr("Search")),
      m_placeHolderAlignment(Qt::AlignCenter),
      m_hasFocus(true),
      isTextEmpty(true)
{
    Q_Q(KSearchLineEdit);
    m_pCompleter = new QCompleter(this);
    m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_pListModel = new QStringListModel(m_List, this);
    m_pCompleter->setModel(m_pListModel);
    q->setCompleter(m_pCompleter);

    QAbstractItemView *popup = m_pCompleter->popup();
    kdk::effects::KShadowHelper::self()->setWidget(popup);
    m_pListViewDelegate = new ListViewDelegate(popup);
    popup->setItemDelegate(m_pListViewDelegate);
    MyStyle *style = new MyStyle;
    popup->setStyle(style);
    setParent(parent);
}

void KSearchLineEditPrivate::doAnimationFinished()
{
    Q_Q(KSearchLineEdit);
    if(m_isSearching)
        q->setTextMargins(iconMargin + m_pIconLabel->width(),0,0,0);
}

void KSearchLineEditPrivate::doEditingFinished()
{
    Q_Q(KSearchLineEdit);
    QString text = q->text();
    if(QString::compare(text.trimmed(),"") != 0)
    {
        bool flag = m_List.contains(text, Qt::CaseInsensitive);
         if(!flag)
         {
             m_List.append(text);
             m_pListModel->setStringList(m_List);
         }
    }
}

void KSearchLineEditPrivate::changeTheme()
{
    Q_Q(KSearchLineEdit);
    adjustHolderRect();
    if(q->text().isEmpty())
        m_pWidget->setGeometry(m_holderRect);
    else
        m_pWidget->setGeometry(m_sideRect);

    QPixmap pixmap = QIcon::fromTheme("search-symbolic").pixmap(QSize(16,16));
    initThemeStyle();

    if(!q->isEnabled())
    {
        QPalette palette = q->palette();
        palette.setColor(QPalette::Text,q->palette().color(QPalette::Disabled,QPalette::ButtonText));
        m_pTextLabel->setPalette(palette);
    }
    else
    {
        QPalette palette = q->palette();
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
            palette.setColor(QPalette::Text,q->palette().color(QPalette::PlaceholderText));
        #else if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
            palette.setColor(QPalette::Text,q->palette().color(QPalette::Shadow));
        #endif

        m_pTextLabel->setPalette(palette);
    }
    if(ThemeController::themeMode() == LightTheme)
    {

        m_pIconLabel->setPixmap(pixmap);
        auto palette = qApp->palette();
        palette.setColor(QPalette::Base,Qt::transparent);
        m_pCompleter->popup()->setPalette(palette);
        m_pCompleter->popup()->setBackgroundRole(QPalette::Base);
        m_pCompleter->popup()->setAutoFillBackground(true);

    }
    else
    {
        m_pIconLabel->setPixmap(drawSymbolicColoredPixmap(pixmap));

        auto palette = qApp->palette();
        palette.setColor(QPalette::Base,Qt::transparent);
        m_pCompleter->popup()->setPalette(palette);
        m_pCompleter->popup()->setBackgroundRole(QPalette::Base);
        m_pCompleter->popup()->setAutoFillBackground(true);

        QPalette pal=qApp->palette();
        m_pTextLabel->setPalette(pal);
    }
}

void KSearchLineEditPrivate::init()
{
    Q_Q(KSearchLineEdit);
    m_isSearching = false;
    m_pTextLabel = new QLabel(tr("Search"));
    m_pIconLabel = new QLabel;
    m_pIconLabel->setScaledContents(true);
    QPixmap pixmap = QIcon::fromTheme("search-symbolic").pixmap(QSize(16,16));
    m_pIconLabel->setPixmap(pixmap);
    m_pIconLabel->setFixedSize(QSize(16,16));
    m_pWidget =new QWidget(q);
    m_pWidget->setFocusPolicy(Qt::NoFocus);
    m_pWidgetLayout = new QHBoxLayout();
    m_pWidgetLayout->setContentsMargins(0,0,0,0);
    m_pWidgetLayout->setMargin(0);
    m_pWidgetLayout->setSpacing(0);
    m_pWidgetLayout->addWidget(m_pIconLabel,Qt::AlignVCenter);
    m_pWidgetLayout->addSpacing(icontextSpacing);
    m_pWidgetLayout->addWidget(m_pTextLabel,Qt::AlignVCenter);
    m_pWidgetLayout->addStretch();
    m_pWidgetLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    m_pWidget->setLayout(m_pWidgetLayout);
    m_pAnimation = new QPropertyAnimation(m_pWidget,"geometry");
    m_pAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_pAnimation->setDuration(100);
    q->setTextMargins(iconMargin + m_pIconLabel->width(),0,0,0);
}

ListViewDelegate::ListViewDelegate(QObject *parent):QStyledItemDelegate(parent)
{

}


void ListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect;
    rect.setX(option.rect.x());
    rect.setY(option.rect.y()+1);
    rect.setWidth(option.rect.width());
    rect.setHeight(option.rect.height());

    const qreal radius = 6;
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

    painter->setRenderHint(QPainter::Antialiasing);
    QColor color = Qt::transparent;
    QColor fontColor = qApp->palette().color(QPalette::ButtonText);

    if(ThemeController::themeMode() == ThemeFlag::LightTheme){
        if(!(option.state & QStyle::State_Enabled))
        {
            color=QColor("#FFB3B3B3");
        }
        else if(((option.state & QStyle::State_HasFocus) || (option.state & QStyle::State_Selected)|| (option.state & QStyle::State_MouseOver)))
            {
                if(option.state.testFlag(QStyle::State_HasFocus) && option.state.testFlag(QStyle::State_Selected)) //QStyle::State_Enabled
                {
                    color = option.palette.highlight().color();
                    fontColor = QColor(255,255,255);
                }
                else if((option.state & QStyle::State_MouseOver))
                {
                    color = option.palette.highlight().color();
                    fontColor = QColor(255,255,255);
                }
                else {
                    color = option.palette.windowText().color();
                    color.setAlphaF(0.05);
                }
                painter->save();
                painter->setPen(QPen(Qt::NoPen));
                painter->setBrush(color);
                painter->drawPath(path);
                painter->restore();
            }
    }
    else{

        if(!(option.state & QStyle::State_Enabled))
        {
            color=QColor("#FFB3B3B3");
        }
        else if(((option.state & QStyle::State_HasFocus) || (option.state & QStyle::State_Selected)|| (option.state & QStyle::State_MouseOver)))
        {
            if(option.state.testFlag(QStyle::State_HasFocus) && option.state.testFlag(QStyle::State_Selected)) //QStyle::State_Enabled
            {
                color = option.palette.highlight().color();
                fontColor = QColor(255,255,255);
            }
            else if((option.state & QStyle::State_MouseOver))
            {
                color = option.palette.highlight().color();
                fontColor = QColor(255,255,255);
            }
            else {
                color = option.palette.windowText().color();
                color.setAlphaF(0.15);
            }
            painter->save();
            painter->setPen(QPen(Qt::NoPen));
            painter->setBrush(color);
            painter->drawPath(path);
            painter->restore();
        }
    }
    QPen pen;
    pen.setWidth(1);
    pen.setColor(fontColor);
    painter->setPen(pen);
    QFontMetrics fm = option.fontMetrics;
    auto str = index.model()->data(index,Qt::DisplayRole).toString();
    if(fm.width(str) >= rect.width()-12 )
    {
        if(option.state.testFlag(QStyle::State_MouseOver))
            QToolTip::showText(QCursor::pos(),str,qobject_cast<QWidget*>(const_cast<QWidget*>(option.widget)));
        else
            QToolTip::hideText();

        str=fm.elidedText(str,Qt::ElideRight,rect.width()-12);
    }
    painter->drawText(rect.adjusted(12,0,0,0),Qt::AlignLeft|Qt::AlignVCenter,str);
}

QSize ListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size;
    size.setWidth(option.widget->width());
    size.setHeight(Parmscontroller::parm(Parmscontroller::Parm::PM_SearchLineEditItemHeight));
    return size;
}


void MyStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case CE_ShapedFrame:
    {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::HighQualityAntialiasing);
        QColor color;
        if(ThemeController::themeMode() == LightTheme)
        {
            color = QColor("#262626");
            color.setAlphaF(0.15);
            painter->setBrush(QColor("#FFFFFF"));
        }
        else
        {
            color = QColor("#333333");
            painter->setBrush(color);
        }
        painter->setPen(color);
        painter->drawRoundedRect(option->rect,8,8);
        break;
    }
    default:
        QProxyStyle::drawControl(element,option,painter,widget);
    }
}

QRect MyStyle::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{

    switch (element)
    {
    case SE_ShapedFrameContents:
        return QProxyStyle::subElementRect(element,option,widget).adjusted(0,4,0,2);
    default:
        return  QProxyStyle::subElementRect(element,option,widget);
    }

}

}
#include "ksearchlineedit.moc"
#include "moc_ksearchlineedit.cpp"
