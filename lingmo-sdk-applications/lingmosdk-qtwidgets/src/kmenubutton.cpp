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

#include "kmenubutton.h"
#include <QAction>
#include <QPainter>
#include "parmscontroller.h"

namespace kdk
{

class KMenuButtonPrivate:public QObject
{
    Q_DECLARE_PUBLIC(KMenuButton)
    Q_OBJECT

public:
    KMenuButtonPrivate(KMenuButton*parent);

private:
    KMenuButton* q_ptr;
    QMenu *m_pMenu;
    QAction *m_pSettingAction;
    QAction *m_pThemeAction;
    QAction *m_pAssistAction;
    QAction *m_pAboutAction;
    QAction *m_pQuitAction;
    QMenu *m_pThemeMenu;
    QAction *m_pAutoAction;
    QAction *m_pLightAction;
    QAction *m_pDarkAction;
};

KMenuButton::KMenuButton(QWidget *parent)
    :QToolButton(parent),
      d_ptr(new KMenuButtonPrivate(this))
{
    Q_D(KMenuButton);

    //setArrowType(Qt::NoArrow);
    d->m_pMenu =new QMenu(this);
    setMenu(d->m_pMenu);
    setToolTip(tr("Options"));
    d->m_pSettingAction = new QAction(tr("Setting"),d->m_pMenu);
    d->m_pThemeAction = new QAction(tr("Theme"),d->m_pMenu);
    d->m_pAssistAction = new QAction(tr("Assist"),d->m_pMenu);
    d->m_pAboutAction = new QAction(tr("About"),d->m_pMenu);
    d->m_pQuitAction = new QAction(tr("Quit"),d->m_pMenu);
    QList<QAction*> actionList;
    actionList<<d->m_pSettingAction<<d->m_pThemeAction<<d->m_pAssistAction<<d->m_pAboutAction<<d->m_pQuitAction;
    d->m_pMenu->addActions(actionList);
    setPopupMode(QToolButton::InstantPopup);

    d->m_pThemeMenu = new QMenu(this);
    d->m_pAutoAction = new QAction(tr("Auto"),d->m_pThemeMenu);
    d->m_pAutoAction->setCheckable(true);
    d->m_pLightAction = new QAction(tr("Light"),d->m_pThemeMenu);
    d->m_pLightAction->setCheckable(true);
    d->m_pDarkAction = new QAction(tr("Dark"),d->m_pThemeMenu);
    d->m_pDarkAction->setCheckable(true);
    QActionGroup * group = new QActionGroup(this);
    group->addAction(d->m_pAutoAction);
    group->addAction(d->m_pLightAction);
    group->addAction(d->m_pDarkAction);

    QList<QAction*> list;
    list<<d->m_pAutoAction<<d->m_pLightAction<<d->m_pDarkAction;
    d->m_pThemeMenu->addActions(list);
    d->m_pThemeAction->setMenu(d->m_pThemeMenu);

    setIcon(QIcon::fromTheme("open-menu-symbolic"));
    setProperty("isWindowButton", 0x1);
    setProperty("useIconHighlightEffect", 0x2);
    setAutoRaise(true);
    changeTheme();
    connect(m_gsetting,&QGSettings::changed,this,[=](){changeTheme();});
    connect(Parmscontroller::self(),&Parmscontroller::modeChanged,this,[=](){
        updateGeometry();
        update();
    });
}

KMenuButton::~KMenuButton()
{

}

QMenu *KMenuButton::menu()
{
    Q_D(KMenuButton);
    return d->m_pMenu;
}

QMenu *KMenuButton::themeMenu()
{
    Q_D(KMenuButton);
    return d->m_pThemeMenu;
}

QAction *KMenuButton::settingAction()
{
    Q_D(KMenuButton);
    return d->m_pSettingAction;
}

QAction *KMenuButton::themeAction()
{
    Q_D(KMenuButton);
    return d->m_pThemeAction;
}

QAction *KMenuButton::assistAction()
{
    Q_D(KMenuButton);
    return d->m_pAssistAction;
}

QAction *KMenuButton::aboutAction()
{
    Q_D(KMenuButton);
    return d->m_pAboutAction;
}

QAction *KMenuButton::quitAction()
{
    Q_D(KMenuButton);
    return d->m_pQuitAction;
}

QAction *KMenuButton::autoAction()
{
    Q_D(KMenuButton);
    return d->m_pAutoAction;
}

QAction *KMenuButton::lightAction()
{
    Q_D(KMenuButton);
    return d->m_pLightAction;
}

QAction *KMenuButton::darkAction()
{
    Q_D(KMenuButton);
    return d->m_pDarkAction;
}

void KMenuButton::changeTheme()
{
    initThemeStyle();
}

void KMenuButton::paintEvent(QPaintEvent *painteEvent)
{
    QToolButton::paintEvent(painteEvent);
}

QSize KMenuButton::sizeHint() const
{
    auto size = QToolButton::sizeHint();
    size.setHeight(Parmscontroller::parm(Parmscontroller::Parm::PM_PushButtonHeight));
    size.setWidth(Parmscontroller::parm(Parmscontroller::Parm::PM_PushButtonHeight));
    return size;
}

KMenuButtonPrivate::KMenuButtonPrivate(KMenuButton *parent)
    :q_ptr(parent)
{
    setParent(parent);
}

}

#include "kmenubutton.moc"
#include "moc_kmenubutton.cpp"
