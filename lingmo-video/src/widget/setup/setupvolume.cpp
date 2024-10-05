#include "setupvolume.h"

#include "global/global.h"
#include "../checkbox.h"

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace Global;

SetupVolume::SetupVolume(QWidget *parent) :
    ThemeWidget(parent)
{
    initLayout();

    // 全局音量和标准化功能先隐藏
    m_title2Label->hide();
    m_globalVolumeCheckBox->hide();
    m_standerdVolumeCheckBox->hide();

    QStringList items;
    //  音频输出默认先设置这几个，具体怎么拿到硬件输出后面需要加上
    items << "pulse" << "alsa";
    m_audioOutputDriver->addItems(items);

    initConnect();
    connectThemeSetting();
}

SetupVolume::~SetupVolume()
{
}

void SetupVolume::initData()
{
    // 初始化输出设备
    QString audio_out = g_config->audioOut.first;
    m_audioOutputDriver->setCurrentText(audio_out);

    m_globalVolumeCheckBox->setChecked(g_config->globalVolume.first);
    m_standerdVolumeCheckBox->setChecked(g_config->standardVolume.first);
}

void SetupVolume::setBlackTheme()
{
    m_title1Label->setStyleSheet(QString("color:rgb(255,255,255);"));
    m_title2Label->setStyleSheet(QString("color:rgb(255,255,255);"));

//    m_audioOutputDriver->setStyleSheet(QString("color:rgb(249,249,249);background-color:rgb(64,64,64);"));
    m_globalVolumeCheckBox->setStyleSheet(QString("color:rgb(249,249,249);"));
    m_standerdVolumeCheckBox->setStyleSheet(QString("color:rgb(249,249,249);"));
}

void SetupVolume::setDefaultTheme()
{
    m_title1Label->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_title2Label->setStyleSheet(QString("color:rgb(38,38,38);"));

//    m_audioOutputDriver->setStyleSheet(QString("color:rgb(38,38,38);background-color:rgb(240,240,240);"));
    m_globalVolumeCheckBox->setStyleSheet(QString("color:rgb(38,38,38);"));
    m_standerdVolumeCheckBox->setStyleSheet(QString("color:rgb(38,38,38);"));
}

void SetupVolume::setWidgetFont(QString family, int size)
{
    QFont f(family);
    f.setPointSize(size + 2);
    m_title1Label->setFont(f);
    m_title2Label->setFont(f);
    f.setPointSize(size);
    m_audioOutputDriver->setFont(f);
    m_globalVolumeCheckBox->setFont(f);
    m_standerdVolumeCheckBox->setFont(f);
}

void SetupVolume::initLayout()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(30, 10, 8, 8);
    lay->setSpacing(12);

    m_title1Label = new QLabel(tr("Output driver"));
    m_title1Label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(m_title1Label);

    {
        m_hboxLayout = new QHBoxLayout;
        m_hboxLayout->setContentsMargins(0, 0, 15, 0);
        m_hboxLayout->setSpacing(8);

        m_audioOutputDriver = new QComboBox;
        m_hboxLayout->addWidget(m_audioOutputDriver);

        lay->addLayout(m_hboxLayout);
    }

    m_title2Label = new QLabel(tr("Volume contral"));
    m_title2Label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(m_title2Label);

    m_globalVolumeCheckBox = new CheckBox;
    m_globalVolumeCheckBox->setText(tr("Global volume"));
    lay->addWidget(m_globalVolumeCheckBox);

    m_standerdVolumeCheckBox = new CheckBox;
    m_standerdVolumeCheckBox->setText(tr("Default volume standardization"));
    lay->addWidget(m_standerdVolumeCheckBox);

    lay->addStretch();
}

void SetupVolume::initConnect()
{
    connect(m_audioOutputDriver, &QComboBox::currentTextChanged, [&](QString dev){g_config->audioOut.second = dev;});
    connect(m_globalVolumeCheckBox, &QCheckBox::toggled, [&](bool checked){g_config->globalVolume.second = checked;});
    connect(m_standerdVolumeCheckBox, &QCheckBox::toggled, [&](bool checked){g_config->standardVolume.second = checked;});
}
