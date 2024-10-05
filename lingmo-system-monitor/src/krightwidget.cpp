/*
 * Copyright (C) 2021 LingmoSoft Co., Ltd.
 *
 * Authors:
 *  Yang Min yangmin@lingmoos.cn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "krightwidget.h"
#include "macro.h"

#include <QProcess>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QKeyEvent>
#include <QDebug>
#include <QDBusInterface>
#include <QDBusMessage>
#include "util.h"

QString KRightWidget::searchText = "";

KRightWidget::KRightWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->installEventFilter(this);
    initUI();
    initConnections();
}

KRightWidget::~KRightWidget()
{
    if (m_animationSearch) {
        delete m_animationSearch;
        m_animationSearch = nullptr;
    }
    if (m_searchTimer) {
        disconnect(m_searchTimer, SIGNAL(timeout()), this, SLOT(onRefreshSearchResult()));
        if(m_searchTimer->isActive()) {
            m_searchTimer->stop();
        }
    }
    if (m_styleSettings) {
        delete m_styleSettings;
        m_styleSettings = nullptr;
    }
}

void KRightWidget::initUI()
{
    // 初始化布局
    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setContentsMargins(0,0,0,0);
    m_mainLayout->setSpacing(16);
    m_titleLayout = new QHBoxLayout();
    m_titleLayout->setContentsMargins(0,0,0,0);
    m_titleLayout->setSpacing(0);
    m_searchLayout = new QHBoxLayout();
    m_searchLayout->setContentsMargins(0,0,0,0);
    m_searchLayout->setSpacing(0);

    // 初始化控件
    m_btnOption  = new QToolButton(this);
    m_btnOption->setToolTip(tr("option"));
    m_btnOption->setProperty("isWindowButton", 0x01);
    m_btnOption->setProperty("useIconHighlightEffect", 0x2);
    m_btnOption->setIcon(QIcon::fromTheme("open-menu-symbolic"));
    m_btnOption->setAutoRaise(true);
    m_btnMin      = new QToolButton(this);
    m_btnMin->setToolTip(tr("minimize"));
    m_btnMin->setProperty("isWindowButton", 0x01);
    m_btnMin->setProperty("useIconHighlightEffect", 0x2);
    m_btnMin->setAutoRaise(true);
    m_btnMin->setIcon(QIcon::fromTheme("window-minimize-symbolic"));
    m_btnMax      = new QToolButton(this);
    m_btnMax->setToolTip(tr("maximize"));
    m_btnMax->setProperty("isWindowButton", 0x01);
    m_btnMax->setProperty("useIconHighlightEffect", 0x2);
    m_btnMax->setAutoRaise(true);
    m_btnMax->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
    m_btnClose    = new QToolButton(this);
    m_btnClose->setToolTip(tr("close"));
    m_btnClose->setProperty("isWindowButton", 0x02);
    m_btnClose->setProperty("useIconHighlightEffect", 0x08);
    m_btnClose->setAutoRaise(true);
    m_btnClose->setIcon(QIcon::fromTheme("window-close-symbolic"));

    m_mainMenu = new QMenu(this);
    m_mainMenu->setMinimumWidth(160);
    m_mainMenu->addAction(tr("Help"));
    m_mainMenu->addAction(tr("About"));
    m_mainMenu->addAction(tr("Exit"));
    m_btnOption->setMenu(m_mainMenu);
    m_btnOption->setPopupMode(QToolButton::InstantPopup);

    connect(m_mainMenu,&QMenu::triggered,this,[=](QAction *action){
        qDebug() << Q_FUNC_INFO << action->text();
        if(action->text() == tr("About")){
            KAboutDialog *k_aboutDialog = new KAboutDialog(this);
            k_aboutDialog->setAppIcon(QIcon::fromTheme("lingmo-system-monitor"));
            k_aboutDialog->setAppName(tr("Lingmo System Monitor"));
            k_aboutDialog->setAppVersion(getUsmVersion());
            k_aboutDialog->setAppSupport(k_aboutDialog->appSupport());
            k_aboutDialog->exec();
        }else if(action->text() == tr("Help")){
            QString service_name = "com.lingmoUserGuide.hotel_" + QString::number(getuid());
            QDBusInterface *interface = new QDBusInterface(service_name,
                                                           LINGMO_USER_GUIDE_PATH,
                                                           LINGMO_USER_GUIDE_INTERFACE,
                                                           QDBusConnection::sessionBus(),
                                                           this);
            QDBusMessage msg = interface->call("showGuide", "lingmo-system-monitor");
        }else if(action->text() == tr("Exit")){
            this->onCloseBtnClicked();
        }
    });

    // 搜索框
    m_searchEditNew = new QLineEdit();
    m_searchEditNew->setFixedSize(SPECIALWIDTH, NORMALHEIGHT);
    m_searchEditNew->installEventFilter(this);
    m_searchEditNew->setContextMenuPolicy(Qt::NoContextMenu);

    m_queryText=new QLabel();
    m_queryText->setText(tr("Search"));
    m_queryWid=new QWidget(m_searchEditNew);
    m_queryWid->setFocusPolicy(Qt::NoFocus);

    m_queryIcon = new QLabel();
    m_queryIcon->setPixmap(QIcon::fromTheme("preferences-system-search-symbolic").pixmap(QSize(16, 16)));
    m_queryIcon->setProperty("useIconHighlightEffect", 0x2);

    QHBoxLayout* queryWidLayout = new QHBoxLayout;
    queryWidLayout->setContentsMargins(4,4,0,0);
    queryWidLayout->setAlignment(Qt::AlignJustify);
    queryWidLayout->setSpacing(8);
    m_queryWid->setLayout(queryWidLayout);

    m_queryIcon->setFixedWidth(SEARCHBUTTON);
    queryWidLayout->addWidget(m_queryIcon);
    queryWidLayout->addWidget(m_queryText);

    queryWidLayout->setAlignment(m_queryIcon,Qt::AlignVCenter);
    queryWidLayout->setAlignment(m_queryText,Qt::AlignVCenter);

    m_rectSeachAnimationBegin.setX((m_searchEditNew->width() - (m_queryIcon->width()+m_queryText->width()+18))/2);
    m_rectSeachAnimationBegin.setY(0);
    m_rectSeachAnimationBegin.setWidth(m_queryIcon->width()+m_queryText->width()+18);
    m_rectSeachAnimationBegin.setHeight((m_searchEditNew->height() + 30)/2);
    m_rectSeachAnimationEnd.setX(6);
    m_rectSeachAnimationEnd.setY(0);
    m_rectSeachAnimationEnd.setWidth(m_queryIcon->width()+5);
    m_rectSeachAnimationEnd.setHeight((m_searchEditNew->height()+30)/2);
    m_queryWid->setGeometry(m_rectSeachAnimationBegin.x(), m_rectSeachAnimationBegin.y(),
        m_rectSeachAnimationBegin.width(), m_rectSeachAnimationBegin.height());   //设置显示label的区域
    m_animationSearch = new QPropertyAnimation(m_queryWid, "geometry");
    m_animationSearch->setDuration(50);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchLayout->addWidget(m_searchEditNew, 0, Qt::AlignHCenter);

    m_titleLayout->setContentsMargins(140,4,4,0);
    m_titleLayout->setSpacing(4);
    m_titleLayout->addLayout(m_searchLayout);
    m_titleLayout->addWidget(m_btnOption);
    m_titleLayout->addWidget(m_btnMin);
    m_titleLayout->addWidget(m_btnMax);
    m_titleLayout->addWidget(m_btnClose);

    m_titleWidget = new QWidget();
    QVBoxLayout *titleVLayout = new QVBoxLayout();
    titleVLayout->setContentsMargins(0,0,0,0);
    titleVLayout->setSpacing(0);
    titleVLayout->addLayout(m_titleLayout);
    titleVLayout->addSpacing(6);
    m_titleWidget->setLayout(titleVLayout);

    m_mainLayout->addWidget(m_titleWidget);
    m_stackedWidget = new QStackedWidget();
    m_mainLayout->addWidget(m_stackedWidget);

    this->setLayout(m_mainLayout);
    initThemeMode();
}

void KRightWidget::initThemeMode()
{
    const QByteArray idd(THEME_QT_SCHEMA);
    if(QGSettings::isSchemaInstalled(idd)) {
        m_styleSettings = new QGSettings(idd);
    }
    if (m_styleSettings) {
        //监听主题改变
        connect(m_styleSettings, &QGSettings::changed, this, [=](const QString &key)
        {
            if (key == "styleName") {
            } else if ("iconThemeName" == key) {
                if (m_queryIcon) {
                    m_queryIcon->setPixmap(QIcon::fromTheme("preferences-system-search-symbolic").pixmap(QSize(16, 16)));
                }
            }
        });
    }
}

void KRightWidget::addPanel(QWidget* pWidget, QString strName, QString strIcon, int nPanelIdx)
{
    Q_UNUSED(strName);
    Q_UNUSED(strIcon);
    if (!pWidget)
        return;
    int nIndex = m_stackedWidget->indexOf(pWidget);
    if (nIndex == -1) { // statckedwidget not contains pWidget
        if (nPanelIdx >= 0) {
            m_stackedWidget->insertWidget(nPanelIdx, pWidget);
        } else {
            m_stackedWidget->addWidget(pWidget);
        }
    }
}

void KRightWidget::initConnections()
{
    connect(m_btnClose, SIGNAL(clicked()), this, SLOT(onCloseBtnClicked()));
    connect(m_btnMin, SIGNAL(clicked()), this, SLOT(onMinBtnClicked()));
    connect(m_btnMax, SIGNAL(clicked()), this, SLOT(onMaxBtnClicked()));
    connect(m_animationSearch,&QPropertyAnimation::finished,this,&KRightWidget::animationFinishedSlot);
    connect(m_searchTimer, SIGNAL(timeout()), this, SLOT(onRefreshSearchResult()));
    connect(m_searchEditNew,&QLineEdit::textChanged,this,&KRightWidget::handleSearchTextChanged,Qt::QueuedConnection);
}

void KRightWidget::createAboutDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_aboutDlg = new USMAboutDialog(this);

    QApplication::restoreOverrideCursor();
}

void KRightWidget::onMinBtnClicked()
{
    emit minimizeWindow();
}

void KRightWidget::onMaxBtnClicked()
{
    emit maximizeWindow();
}

void KRightWidget::onCloseBtnClicked()
{
    window()->close();
}

void KRightWidget::onUpdateMaxBtnState()
{
    if (m_btnMax) {
        if(window()->isMaximized()) {
            m_btnMax->setIcon(QIcon::fromTheme("window-restore-symbolic"));
            m_btnMax->setToolTip(tr("restore"));
        } else {
            m_btnMax->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
            m_btnMax->setToolTip(tr("maximize"));
        }
    }
}

void KRightWidget::updateMaxBtn(bool max)
{
    if (max) {
        m_btnMax->setIcon(QIcon::fromTheme("window-restore-symbolic"));
        m_btnMax->setToolTip(tr("restore"));
    } else {
        m_btnMax->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        m_btnMax->setToolTip(tr("maximize"));
    }
}

void KRightWidget::paintEvent(QPaintEvent *)
{
    QPainterPath path;

    QPainter painter(this);
    painter.setOpacity(1);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setClipping(true);
    painter.setPen(Qt::transparent);

    path.addRect(this->rect());
    path.setFillRule(Qt::WindingFill);
    painter.setBrush(this->palette().base());
    painter.setPen(Qt::transparent);

    painter.drawPath(path);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void KRightWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

bool KRightWidget::eventFilter(QObject *obj, QEvent *event)    //set the esc and tab pressbutton effect
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);
        QWidget *searchParentWid = static_cast<QWidget*>(m_searchEditNew->parent());
        QPoint  searchPoint      = searchParentWid->mapFromGlobal(mEvent->globalPos());
        //qInfo()<<"searchPoint:"<<searchPoint<<","<<m_searchEditNew->geometry()<<","<<m_isSearching;
        if (!m_searchEditNew->geometry().contains(searchPoint)) {
            if (m_isSearching == true) {
                m_searchEditNew->setFocus();
                m_searchEditNew->clearFocus();
            } else {
                m_searchEditNew->clear();
                m_searchEditNew->clearFocus();
                if(m_searchEditNew->text().isEmpty()) {
                    showSearchFocusAnimation(false);
                }
            }
        }
    }

    if (event->type() == QEvent::KeyPress) {
        if (obj == this) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape || keyEvent->key() == Qt::Key_Tab) {
                m_searchEditNew->clear();
                m_searchEditNew->clearFocus();
                showSearchFocusAnimation(false);
            }
        } else if (obj == m_searchEditNew) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Tab) {
                m_searchEditNew->clear();
                m_searchEditNew->clearFocus();
                showSearchFocusAnimation(false);
            }
        }
    }

    if (obj == m_searchEditNew) {
        if (event->type() == QEvent::FocusIn) {
            if(m_searchEditNew->text().isEmpty()) {
                showSearchFocusAnimation(true);
            }
        } else if(event->type() == QEvent::FocusOut && m_isSearching == false) {
            m_searchEditNew->clear();
            m_searchEditNew->clearFocus();
            if(m_searchEditNew->text().isEmpty()) {
                showSearchFocusAnimation(false);
            }
            m_isSearching=false;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void KRightWidget::onSwichPanel(int nIndex)
{
    if (nIndex >= m_stackedWidget->count())
        return;
    m_searchEditNew->clear();
    m_searchEditNew->clearFocus();
    if(m_searchEditNew->text().isEmpty()) {
        showSearchFocusAnimation(false);
    }
    emit this->searchSignal("");
    m_stackedWidget->setCurrentIndex(nIndex);
}

int KRightWidget::currentIndex()
{
    return m_stackedWidget->currentIndex();
}

void KRightWidget::showSearchFocusAnimation(bool bIn)
{
    static bool bInFocus = false;
    if (bInFocus == bIn) {
        return ;
    }
    bInFocus = bIn;
    if (bIn) {
        m_animationSearch->stop();
        m_animationSearch->setStartValue(m_rectSeachAnimationBegin);
        m_animationSearch->setEndValue(m_rectSeachAnimationEnd);
        m_animationSearch->setEasingCurve(QEasingCurve::OutQuad);
        m_animationSearch->start();
    } else {
        m_queryText->adjustSize();
        m_animationSearch->setStartValue(m_rectSeachAnimationEnd);
        m_animationSearch->setEndValue(m_rectSeachAnimationBegin);
        m_animationSearch->setEasingCurve(QEasingCurve::InQuad);
        m_animationSearch->start();
    }
}

void KRightWidget::animationFinishedSlot()
{
    if(m_queryText->parent() == m_queryWid) {
        m_queryWid->layout()->removeWidget(m_queryText);
        m_queryText->setParent(nullptr);
        m_searchEditNew->setTextMargins(30,1,0,1);
    } else {
        m_queryWid->layout()->addWidget(m_queryText);
    }
}

void KRightWidget::onRefreshSearchResult()
{
    if (m_searchEditNew->text() == m_searchTextCache) {
        //qInfo()<<"Search Keyword:"<<m_searchTextCache;
        emit this->searchSignal(m_searchTextCache);
    }
}

void KRightWidget::handleSearchTextChanged()
{
    m_searchTextCache = m_searchEditNew->text();
    KRightWidget::searchText = m_searchEditNew->text();
    if (m_searchTextCache.isEmpty()) {
        m_isSearching = false;
    } else {
        m_isSearching = true;
    }
    if (m_searchTimer->isActive()) {
        m_searchTimer->stop();
    }
    m_searchTimer->start(300);
}

void KRightWidget::changeTitleWidget(bool isTablet)
{
    if (isTablet) {
        m_titleWidget->setFixedHeight(64);
        m_btnOption->setFixedSize(48, 48);
        m_btnMin->setFixedSize(48, 48);
        m_btnClose->setFixedSize(48, 48);
        m_btnMax->setVisible(false);
    } else {
        m_titleWidget->setFixedHeight(40);
        m_btnOption->setFixedSize(30, 30);
        m_btnMin->setFixedSize(30, 30);
        m_btnMax->setFixedSize(30, 30);
        m_btnClose->setFixedSize(30, 30);
        m_btnMax->setVisible(true);
    }
}
