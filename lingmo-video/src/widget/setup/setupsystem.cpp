#include "setupsystem.h"

#include "global/global.h"
#include "../checkbox.h"

#include <QLabel>
#include <QVBoxLayout>

using namespace Global;

SetupSystem::SetupSystem(QWidget *parent) :
    ThemeWidget(parent)
{
    initLayout();

    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(16);
    m_title1Label->setFont(f);

    f.setPixelSize(14);
    m_mini2trayCheckBox->setFont(f);
    m_pauseWhenMiniCheckBox->setFont(f);
    m_keepStateCheckBox->setFont(f);

    initConnect();
    connectThemeSetting();
}

SetupSystem::~SetupSystem()
{
}


/** **********************************************
 * 主题颜色修改
 *************************************************/
void SetupSystem::setBlackTheme()
{
    m_title1Label->setStyleSheet(QString("color:rgb(255,255,255);"));

    m_mini2trayCheckBox->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_pauseWhenMiniCheckBox->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_keepStateCheckBox->setStyleSheet(QString("color:rgb(249,249,249);"));
}

void SetupSystem::setDefaultTheme()
{
    m_title1Label->setStyleSheet(QString("color:rgb(38,38,38);"));

    m_mini2trayCheckBox->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_pauseWhenMiniCheckBox->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_keepStateCheckBox->setStyleSheet(QString("color:rgb(38,38,38);"));
}

void SetupSystem::setWidgetFont(QString family, int size)
{
    QFont f(family);
    f.setPointSize(size + 2);
    m_title1Label->setFont(f);

    f.setPointSize(size);
    m_keepStateCheckBox->setFont(f);
    m_mini2trayCheckBox->setFont(f);
    m_pauseWhenMiniCheckBox->setFont(f);
}

void SetupSystem::initLayout()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(30, 10, 8, 8);
    lay->setSpacing(0);

    m_title1Label = new QLabel(tr("Window"));
    m_title1Label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(m_title1Label);

    m_mini2trayCheckBox = new CheckBox;
    m_mini2trayCheckBox->setText(tr("Minimize to system tray icon"));
    if (!isWayland)
    	lay->addWidget(m_mini2trayCheckBox);

    m_pauseWhenMiniCheckBox = new CheckBox;
    m_pauseWhenMiniCheckBox->setText(tr("Pause video playback when minimized"));
    lay->addWidget(m_pauseWhenMiniCheckBox);

    m_keepStateCheckBox = new CheckBox;
    m_keepStateCheckBox->setText(tr("After sleep/sleep/lock screen and wake up, keep playing state"));
    lay->addWidget(m_keepStateCheckBox);

    lay->addStretch();
}

void SetupSystem::initData()
{
    m_mini2trayCheckBox->setChecked(g_config->miniToTray.first);
    m_pauseWhenMiniCheckBox->setChecked(g_config->pauseWhenMini.first);
    m_keepStateCheckBox->setChecked(g_config->keepStateWhenWakeup.first);
}

void SetupSystem::initConnect()
{
    connect(m_mini2trayCheckBox, &QCheckBox::clicked, [&](bool checked){
        g_config->miniToTray.second = checked;
    });
    connect(m_pauseWhenMiniCheckBox, &QCheckBox::clicked, [&](bool checked){
        g_config->pauseWhenMini.second = checked;
    });
    connect(m_keepStateCheckBox, &QCheckBox::clicked, [&](bool checked){
        g_config->keepStateWhenWakeup.second = checked;
    });
}
