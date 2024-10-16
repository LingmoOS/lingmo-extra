/*
 * Copyright (C) 2020 LingmoSoft Co., Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntulingmo.com/kobe24_lixiang@126.com
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

#include "renicedialog.h"
#include "../util.h"
#include "../macro.h"
#include "../xatom-helper.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include <QPainterPath>

ReniceDialog::ReniceDialog(const QString &procName, const QString &procId, QWidget *parent)
    : QDialog(parent)
    , m_mousePressed(false)
{
    QString platform = QGuiApplication::platformName();
    if (!platform.startsWith(QLatin1String("wayland"),Qt::CaseInsensitive)) {
        MotifWmHints hints;
        hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
        hints.functions = MWM_FUNC_ALL;
        hints.decorations = MWM_DECOR_BORDER;
        XAtomHelper::getInstance()->setWindowMotifHint(this->winId(), hints);
    }
    //this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    this->setFixedSize(480+SHADOW_LEFT_TOP_PADDING+SHADOW_LEFT_TOP_PADDING, 300+SHADOW_RIGHT_BOTTOM_PADDING+SHADOW_RIGHT_BOTTOM_PADDING);
    this->setAttribute(Qt::WA_DeleteOnClose);

    const QByteArray id(THEME_QT_SCHEMA);
    if(QGSettings::isSchemaInstalled(id))
    {
        styleSettings = new QGSettings(id);
    }

    initThemeStyle();

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0,0,0,0);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setMargin(0);
    m_picTitleIcon = new QLabel(this);
    m_picTitleIcon->setPixmap(QIcon::fromTheme("lingmo-system-monitor").pixmap(24,24));
    m_dlgTitleLable = new QLabel;
    m_strProcName = procName;
    m_strProcId = procId;
    m_dlgTitleLable->setText(tr("Change Priority of Process %1 (PID: %2)").arg(m_strProcName).arg(m_strProcId));
    m_dlgTitleLable->setFixedWidth(400);
    QPushButton *closeButton = new QPushButton(this);
    closeButton->setFlat(true);
    closeButton->setIcon(QIcon::fromTheme("window-close-symbolic"));
    closeButton->setProperty("isWindowButton", 0x2);
    closeButton->setProperty("useIconHighlightEffect", 0x8);
    closeButton->setFixedSize(30, 30);

    QHBoxLayout *title_H_BoxLayout = new QHBoxLayout();
    title_H_BoxLayout->setContentsMargins(8,4,4,0);
    title_H_BoxLayout->setSpacing(10);
    title_H_BoxLayout->addWidget(m_picTitleIcon,0,Qt::AlignLeft);
    title_H_BoxLayout->addWidget(m_dlgTitleLable,0,Qt::AlignLeft);
    title_H_BoxLayout->addWidget(closeButton,0,Qt::AlignRight);

    m_titleLabel = new QLabel();
    m_titleLabel->setFixedWidth(90);
    m_titleLabel->setText(tr("Nice value:"));
    m_valueLabel = new QLabel();
    m_slider = new USlider(Qt::Horizontal);
    m_slider->setRange(-20, 19);
    m_slider->setSingleStep(1);

    h_layout = new QHBoxLayout();
    h_layout->setSpacing(10);
    h_layout->setMargin(0);
    h_layout->setContentsMargins(24,10,24,0);
    h_layout->addWidget(m_titleLabel);
    h_layout->addWidget(m_slider);
    h_layout->addWidget(m_valueLabel);

    m_valueStrLabel = new QLabel;
    m_valueStrLabel->setAlignment(Qt::AlignCenter);
    m_valueStrLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_tipTitle = new QLabel;
    m_tipTitle->setText(tr("Note:"));
    m_tipLabel = new QLabel;
    m_tipLabel->setWordWrap(true);//QLabel自动换行
    m_tipLabel->setFixedSize(388, 90);
    m_tipLabel->setText(tr("The priority of a process is given by its nice value. A lower nice value corresponds to a higher priority."));
    tip_layout = new QHBoxLayout();
    tip_layout->setSpacing(5);
    tip_layout->setMargin(0);
    tip_layout->setContentsMargins(24,0,24,0);
    tip_layout->addWidget(m_tipTitle);
    tip_layout->addWidget(m_tipLabel);

    m_cancelbtn = new QPushButton;
    m_cancelbtn->setFixedSize(184, 36);
    m_cancelbtn->setObjectName("blackButton");
    m_cancelbtn->setFocusPolicy(Qt::NoFocus);
    m_cancelbtn->setText(tr("Cancel"));
    m_changeBtn = new QPushButton;
    m_changeBtn->setFixedSize(184, 36);
    m_changeBtn->setObjectName("blackButton");
    m_changeBtn->setFocusPolicy(Qt::NoFocus);
    m_changeBtn->setText(tr("Change Priority"));

    btn_layout = new QHBoxLayout();
    btn_layout->setMargin(0);
    btn_layout->setSpacing(10);
    btn_layout->setContentsMargins(24,0,24,24);
    btn_layout->addWidget(m_cancelbtn);
    btn_layout->addStretch();
    btn_layout->addWidget(m_changeBtn);


    QVBoxLayout *v_layout = new QVBoxLayout();
    v_layout->setMargin(0);
    v_layout->setSpacing(15);
    v_layout->setContentsMargins(0,0,0,0);
    v_layout->addLayout(h_layout, 0);
    v_layout->addWidget(m_valueStrLabel, 0, Qt::AlignHCenter);
    v_layout->addLayout(tip_layout, 0);
    v_layout->addLayout(btn_layout, 0);

    m_mainLayout->addLayout(title_H_BoxLayout);
    m_mainLayout->addLayout(v_layout);

    connect(m_slider, &QSlider::valueChanged, [=] (int value) {
        m_valueLabel->setText(QString::number(value));
        QString levelStr = getNiceLevelWithPriority(value);
        m_valueStrLabel->setText(levelStr);
    });

//    connect(m_titleBar, SIGNAL(minSignal()), this, SLOT(hide()));
//    connect(m_titleBar, SIGNAL(closeSignal()), this, SLOT(onClose()));
    connect(m_cancelbtn, SIGNAL(clicked(bool)), this, SLOT(onClose()));
    connect(closeButton,SIGNAL(clicked(bool)),this,SLOT(onClose()));
    connect(m_changeBtn, &QPushButton::clicked, [=] (bool b) {
        Q_UNUSED(b);
        emit this->resetReniceValue(m_slider->value());
    });

    //边框阴影效果
//    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
//    shadow_effect->setBlurRadius(5);
//    shadow_effect->setColor(QColor(0, 0, 0, 127));
//    shadow_effect->setOffset(2, 4);
//    this->setGraphicsEffect(shadow_effect);

//    this->moveCenter();

    //QDesktopWidget* desktop = QApplication::desktop();
    //this->move((desktop->width() - this->width())/2, (desktop->height() - this->height())/3);
    onThemeFontChange(fontSize);
}

ReniceDialog::~ReniceDialog()
{
//    delete m_titleBar;
    delete m_valueStrLabel;

    QLayoutItem *child;
    while ((child = h_layout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    while ((child = tip_layout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
    while ((child = btn_layout->takeAt(0)) != 0) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }

    delete m_mainLayout;
    if (styleSettings) {
        delete styleSettings;
        styleSettings = nullptr;
    }
}

void ReniceDialog::onClose()
{
    this->close();
}

void ReniceDialog::loadData(int nice)
{
    m_slider->setValue(nice);
    m_valueLabel->setText(QString::number(nice));

    QString levelStr = getNiceLevelWithPriority(nice);
    m_valueStrLabel->setText(levelStr);
}

void ReniceDialog::moveCenter()
{
    /*QPoint pos = QCursor::pos();
    QRect primaryGeometry;
    for (QScreen *screen : qApp->screens()) {
        if (screen->geometry().contains(pos)) {
            primaryGeometry = screen->geometry();
        }
    }

    if (primaryGeometry.isEmpty()) {
        primaryGeometry = qApp->primaryScreen()->geometry();
    }

    this->move(primaryGeometry.x() + (primaryGeometry.width() - this->width())/2,
               primaryGeometry.y() + (primaryGeometry.height() - this->height())/2);
    this->show();
    this->raise();*/
}


void ReniceDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        this->m_mousePressed = true;
    }

    QDialog::mousePressEvent(event);
}

void ReniceDialog::mouseReleaseEvent(QMouseEvent *event)
{
    this->m_mousePressed = false;

    QDialog::mouseReleaseEvent(event);
}

void ReniceDialog::mouseMoveEvent(QMouseEvent *event)
{
    QDialog::mouseMoveEvent(event);
}

void ReniceDialog::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    QPainter painter(this);

    path.addRect(this->rect());
    path.setFillRule(Qt::WindingFill);
    painter.setBrush(this->palette().base());
    painter.setPen(Qt::transparent);
    painter.drawPath(path);
    QDialog::paintEvent(event);
}

void ReniceDialog::initThemeStyle()
{
    if (!styleSettings) {
        fontSize = DEFAULT_FONT_SIZE;
        return;
    }
    connect(styleSettings,&QGSettings::changed,[=](QString key)
    {
        if("systemFont" == key || "systemFontSize" == key)
        {
            fontSize = styleSettings->get(FONT_SIZE).toString().toFloat();
            this->onThemeFontChange(fontSize);
        } else if ("iconThemeName" == key) {
            if (m_picTitleIcon)
                m_picTitleIcon->setPixmap(QIcon::fromTheme("lingmo-system-monitor").pixmap(24,24));
        }
    });
    fontSize = styleSettings->get(FONT_SIZE).toString().toFloat();
}

void ReniceDialog::onThemeFontChange(qreal lfFontSize)
{
    Q_UNUSED(lfFontSize);
    if (m_dlgTitleLable) {
        QString strProcName = getMiddleElidedText(m_dlgTitleLable->font(), m_strProcName, 200);
        QString strOrigTitle = tr("Change Priority of Process %1 (PID: %2)").arg(strProcName).arg(m_strProcId);
        QString strTitle = getElidedText(m_dlgTitleLable->font(), strOrigTitle, m_dlgTitleLable->width()-2);
        m_dlgTitleLable->setText(strTitle);
        if (strTitle != strOrigTitle) {
            m_dlgTitleLable->setToolTip(strOrigTitle);
        } else {
            m_dlgTitleLable->setToolTip("");
        }
    }
    if (m_cancelbtn) {
        QString strOrigCancel = tr("Cancel");
        QString strCancel = getElidedText(m_cancelbtn->font(), strOrigCancel, m_cancelbtn->width()-4);
        m_cancelbtn->setText(strCancel);
        if (strCancel != strOrigCancel) {
            m_cancelbtn->setToolTip(strOrigCancel);
        } else {
            m_cancelbtn->setToolTip("");
        }
    }
    if (m_changeBtn) {
        QString strOrigChange = tr("Change Priority");
        QString strChange = getElidedText(m_changeBtn->font(), strOrigChange, m_changeBtn->width()-4);
        m_changeBtn->setText(strChange);
        if (strChange != strOrigChange) {
            m_changeBtn->setToolTip(strOrigChange);
        } else {
            m_changeBtn->setToolTip("");
        }
    }
    if (m_titleLabel) {
        QString strOrigNice = tr("Nice value:");
        QString strNice = getElidedText(m_titleLabel->font(), strOrigNice, m_titleLabel->width()-4);
        m_titleLabel->setText(strNice);
        if (strNice != strOrigNice) {
            m_titleLabel->setToolTip(strOrigNice);
        } else {
            m_titleLabel->setToolTip("");
        }
    }
}
