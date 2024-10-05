#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <KF5/KWindowSystem/kwindoweffects.h>

#include <QDebug>
#include <QPainter>
#include <QGSettings>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QListWidgetItem>

#include "global/globalsignal.h"
#include "global/global.h"

#include "setupcodec.h"
#include "setupplay.h"
#include "setupscreenshot.h"
#include "setupshortcut.h"
#include "setupsubtitle.h"
#include "setupsystem.h"
#include "setupvolume.h"

using namespace Global;

SetUpDialog::SetUpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetUpDialog)
{
    ui->setupUi(this);
    initStyle();
    initListWidget();

    if (FOLLOW_SYS_THEME) {
        // 根据主题设置样式
        if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
            slotChangeTheme(false);
        else
            slotChangeTheme(true);
    }
    else {
        slotChangeTheme(true);
    }
    initConnect();
}

SetUpDialog::~SetUpDialog()
{
    delete ui;
}

void SetUpDialog::setIndex(int index)
{
    ui->listWidget->setCurrentRow(index);
}

/** **********************************************
 * 主题颜色修改
 * 说明：所有需要根据主题修改颜色的控件都在此函数设置样式
 * @param: is_black_theme 是否为黑色主题
 *************************************************/
void SetUpDialog::slotChangeTheme(bool is_black_theme)
{
    m_isBlackTheme = is_black_theme;
    QString listHoverColor = "";
    if(is_black_theme)
    {
        m_leftColor.setRgb(19, 19, 20);
//        m_systemPage->setBlackTheme();
        m_playPage->setBlackTheme();
        m_screenshotPage->setBlackTheme();
        m_subtitlePage->setBlackTheme();
        m_volumePage->setBlackTheme();
        m_codecPage->setBlackTheme();
        m_shortcutPage->setBlackTheme();

        ui->right->setStyleSheet("#right{background-color:rgb(38,38,38);}");
        ui->lab_name->setStyleSheet("color:rgb(255,255,255);");
        // 设置左侧list字体颜色
        for(int i=0; i<ui->listWidget->count(); i++)
        {
            QListWidgetItem *it = ui->listWidget->item(i);
            it->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            it->setSizeHint(QSize(116, 32));
            it->setTextColor(QColor(249, 249, 249));
        }
        listHoverColor = "222,222,222";
    }
    else
    {
        m_leftColor.setRgb(249, 249, 249);
//        m_systemPage->setDefaultTheme();
        m_playPage->setDefaultTheme();
        m_screenshotPage->setDefaultTheme();
        m_subtitlePage->setDefaultTheme();
        m_volumePage->setDefaultTheme();
        m_codecPage->setDefaultTheme();
        m_shortcutPage->setDefaultTheme();

        ui->right->setStyleSheet("#right{background-color:#f9f9f9;}");
        ui->lab_name->setStyleSheet("color:rgb(48,49,51);");

        // 设置左侧list字体颜色
        for(int i=0; i<ui->listWidget->count(); i++)
        {
            QListWidgetItem *it = ui->listWidget->item(i);
            it->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            it->setSizeHint(QSize(116, 32));
            it->setTextColor(QColor(89, 89, 89));
        }
        listHoverColor = "26,26,26";
    }
    updateHightLightColor();
    ui->btn_cancel->setProperty("useButtonPalette", true);
    update();
}

bool SetUpDialog::event(QEvent *e)
{
    // 主要处理 palette 事件
    if (e->type() == QEvent::ApplicationPaletteChange) {
        m_highlightColor = QApplication::palette(this).highlight().color();
        updateHightLightColor();
    }
    return QDialog::event(e);
}

void SetUpDialog::showEvent(QShowEvent *e)
{
    m_systemPage->initData();
    m_playPage->initData();
    m_screenshotPage->initData();
    m_subtitlePage->initData();
    m_volumePage->initData();

    resize(620, 400);
    // 设置模糊
    KWindowEffects::enableBlurBehind(winId(), true);
    g_shortcut->makeAllInvalid();
    resetFont();
}

void SetUpDialog::hideEvent(QHideEvent *e)
{
    g_shortcut->makeAllValid();
}

void SetUpDialog::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setPen(Qt::NoPen);
    m_leftColor.setAlphaF(m_windowOpacity);
    QBrush brush = QBrush(m_leftColor);
    p.setBrush(brush);
    p.drawRoundedRect(opt.rect, 0, 0);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SetUpDialog::updateHightLightColor()
{
    QString listHoverColor = m_isBlackTheme ? "222,222,222" : "26,26,26";
    ui->listWidget->setStyleSheet(
                QString("QListWidget{background-color:rgba(19,19,20,0);padding:0px 5px;}"
                        "QListWidget::item:selected{width:116px;border-radius:6px;color:rgb(255,255,255);background-color:%1;}"
                        "QListWidget::item:hover:!selected{width:116px;border-radius:6px;background-color:rgba(%2,30);}")
                        .arg(m_highlightColor.name())
                        .arg(listHoverColor));

    {
//        QPalette tmp_palette = palette();
//        m_systemPage->setPalette(tmp_palette);
    }
}

void SetUpDialog::initStyle()
{
    if (g_gsettings_control_center)
        m_windowOpacity = g_gsettings_control_center->get("transparency").toDouble();
    else
        m_windowOpacity = 1.0;
    setFixedSize(620, 400);
    setAttribute(Qt::WA_TranslucentBackground);
    m_highlightColor = QApplication::palette(this).highlight().color();
    // 标题
    setWindowTitle(tr("Video Player Set up"));

    {
        m_systemPage = new SetupSystem;
        ui->stackedWidget->addWidget(m_systemPage);

        m_playPage = new SetupPlay;
        ui->stackedWidget->addWidget(m_playPage);

        m_screenshotPage = new SetupScreenshot;
        ui->stackedWidget->addWidget(m_screenshotPage);

        m_subtitlePage = new SetupSubtitle;
        ui->stackedWidget->addWidget(m_subtitlePage);

        m_volumePage = new SetupVolume;
        ui->stackedWidget->addWidget(m_volumePage);

        m_codecPage = new SetupCodec;
        ui->stackedWidget->addWidget(m_codecPage);

        m_shortcutPage = new SetupShortcut;
        ui->stackedWidget->addWidget(m_shortcutPage);
    }

    // 左侧列表
    ui->listWidget->setFixedWidth(140);
    ui->listWidget->setSpacing(2);
    // 设置默认选中左侧列表中第一个
    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);

    ui->widget->setAttribute(Qt::WA_TranslucentBackground);
    ui->lab_name->setAttribute(Qt::WA_TranslucentBackground);

    ui->btn_icon->setFixedSize(QSize(26,26));
    ui->btn_icon->setIconSize(QSize(26,26));
    ui->btn_icon->setIcon(QIcon::fromTheme("lingmo-video"));
    ui->btn_icon->setStyleSheet("QPushButton{border:0px;background:transparent;}"
                                "QPushButton::hover{border:0px;background:transparent;}"
                                "QPushButton::pressed{border:0px;background:transparent;}");

    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(16);
    ui->lab_name->setFont(f);
    ui->lab_name->setText(tr("Setup"));

    ui->btn_close->setFixedSize(30, 30);
    ui->btn_close->setProperty("isWindowButton", 0x2);
    ui->btn_close->setProperty("useIconHighlightEffect", 0x8);
    ui->btn_close->setFlat(true);
    ui->btn_close->setIcon(QIcon::fromTheme("window-close-symbolic"));

    connect(ui->btn_close, &QPushButton::clicked, [&](){accept();});

    resetFont();
}

/** **********************************************
 * 初始化左侧列表
 *************************************************/
void SetUpDialog::initListWidget()
{
    ui->listWidget->addItem(" " + tr("System"));
    ui->listWidget->addItem(" " + tr("Play"));
    ui->listWidget->addItem(" " + tr("ScreenShot"));
    ui->listWidget->addItem(" " + tr("Subtitle"));
    ui->listWidget->addItem(" " + tr("Audio"));
    ui->listWidget->addItem(" " + tr("Codec"));
    ui->listWidget->addItem(" " + tr("Shortcut"));
}

void SetUpDialog::initConnect()
{
    if (FOLLOW_SYS_THEME) {
        // 主题改变
        connect(g_user_signal, &GlobalUserSignal::sigTheme, this, [&](int theme){
            switch (theme) {
            case 0:
                if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                    slotChangeTheme(false);
                else
                    slotChangeTheme(true);
                break;
            case 1:
                slotChangeTheme(false);
                break;
            case 2:
                slotChangeTheme(true);
                break;
            default:
                break;
            }
        });
    }
    connect(g_gsettings, &QGSettings::changed, this, [&](QString key){
        if (FOLLOW_SYS_THEME) {
            // 如果不是跟随主题的话直接返回,0是跟随主题
            if (key == "styleName") {
                if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                    slotChangeTheme(false);
                else
                    slotChangeTheme(true);
            }
        }

        // 字体大小改变
        if (key == "systemFontSize" || key == "systemFont") {
            resetFont();
        }
    });

    if (g_gsettings_control_center)
        connect(g_gsettings_control_center, &QGSettings::changed, this, [&](QString key){
            // 如果不是跟随主题的话直接返回
            if (key == "transparency") {
                m_windowOpacity = g_gsettings_control_center->get("transparency").toDouble();
                update();
            }
        });

    connect(ui->listWidget, &QListWidget::currentRowChanged, [&](int index){ui->stackedWidget->setCurrentIndex(index);});
    connect(ui->btn_cancel, &QPushButton::clicked, [&](){m_shortcutPage->clearChange();g_config->clearChange();accept();});
    connect(ui->btn_ok, &QPushButton::clicked, [&](){m_shortcutPage->flushChange();g_config->flushChange();accept();});
}

void SetUpDialog::resetFont()
{
    QString fontFamily = g_gsettings->get("system-font").toString();
    int fontSize = g_gsettings->get("systemFontSize").toDouble();
    double fsize = g_gsettings->get("systemFontSize").toDouble();

    m_systemPage->setWidgetFont(fontFamily, fontSize);
    m_codecPage->setWidgetFont(fontFamily, fontSize);
    m_playPage->setWidgetFont(fontFamily, fontSize);
    m_subtitlePage->setWidgetFont(fontFamily, fontSize);
    m_shortcutPage->setWidgetFont(fontFamily, fontSize);
    m_volumePage->setWidgetFont(fontFamily, fontSize);
    m_screenshotPage->setWidgetFont(fontFamily, fontSize);

    QFont f(fontFamily);
    f.setPointSize(fontSize);
    ui->listWidget->setFont(f);
    ui->btn_cancel->setFont(f);
    ui->btn_ok->setFont(f);

    f.setPointSize(fontSize + 2);
    ui->lab_name->setFont(f);
}
