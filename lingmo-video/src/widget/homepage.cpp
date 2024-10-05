#include "homepage.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "global/global.h"

using namespace Global;

HomePage::HomePage(QWidget *parent) : QWidget(parent)
{
    connect(g_gsettings, &QGSettings::changed, this, &HomePage::gSettingChanged);

    if (g_config->videoOutputType() == GlobalConfig::VO_OPENGL)
        setAttribute(Qt::WA_TranslucentBackground);
    m_background = new QWidget(this);
    m_background->setObjectName("background_widget");
    m_background->setStyleSheet("#background_widget{border-image: url(:/ico/bg.png);}");

    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(16);

    m_btnOpenFile = new QPushButton;
//    m_btnOpenFile->setFont(f);
    connect(m_btnOpenFile, &QPushButton::clicked, [this](){g_user_signal->sigSelectFile();});
    m_btnOpenFile->setText(tr("open file"));
    m_btnOpenFile->setCursor(Qt::PointingHandCursor);
    m_btnOpenFile->setFixedSize(MainButtonSize);
    m_btnOpenFile->setStyleSheet("QPushButton:!hover{background-color:rgba(255,255,255,19);border-radius:4px;color:rgb(255,255,255);}"
                                 "QPushButton:hover{background-color:rgba(255,255,255,25);border-radius:4px;color:rgb(255,255,255);}"
                                 "QPushButton:pressed{background-color:rgba(255,255,255,14);border-radius:4px;color:rgb(255,255,255);}");

    m_btnOpenDir = new QPushButton;
//    m_btnOpenDir->setFont(f);
    connect(m_btnOpenDir, &QPushButton::clicked, [this](){g_user_signal->sigSelectDir();});
    m_btnOpenDir->setText(tr("open dir"));
    m_btnOpenDir->setCursor(Qt::PointingHandCursor);
    m_btnOpenDir->setFixedSize(MainButtonSize);
    m_btnOpenDir->setStyleSheet("QPushButton:!hover{background-color:rgba(255,255,255,19);border-radius:4px;color:rgb(255,255,255);}"
                                "QPushButton:hover{background-color:rgba(255,255,255,25);border-radius:4px;color:rgb(255,255,255);}"
                                "QPushButton:pressed{background-color:rgba(255,255,255,14);border-radius:4px;color:rgb(255,255,255);}");

    QHBoxLayout *lay_btn = new QHBoxLayout;
    lay_btn->setContentsMargins(0, 0, 0, 0);
    lay_btn->setSpacing(8);
    lay_btn->addStretch();
    lay_btn->addWidget(m_btnOpenFile);
    lay_btn->addWidget(m_btnOpenDir);
    lay_btn->addStretch();

    m_btnLogo = new QPushButton;
    m_labLogoName = new QLabel;
    m_labLogoName->setText(tr("Video Player"));
    m_labLogoName->setStyleSheet("color: rgba(249, 249, 249, 204);"
                                 "font: 57 46px \"Noto Sans CJK SC Medium\";");

    QHBoxLayout *lay_logo = new QHBoxLayout;
    lay_logo->setContentsMargins(0, 0, 0, 0);
    lay_logo->setSpacing(5);
    lay_logo->addStretch();
    lay_logo->addWidget(m_btnLogo);
    lay_logo->addWidget(m_labLogoName);
    lay_logo->addStretch();

    m_btnLogo->setFixedSize(QSize(70,70));
    m_btnLogo->setIconSize(QSize(70,70));
    m_btnLogo->setIcon(QIcon::fromTheme("lingmo-video"));
    m_btnLogo->setStyleSheet("QPushButton{border:0px;background:transparent;}"
                             "QPushButton::hover{border:0px;background:transparent;}"
                             "QPushButton::pressed{border:0px;background:transparent;}");

    QVBoxLayout *lay_center = new QVBoxLayout;
    lay_center->setContentsMargins(0, 0, 0, 0);
    lay_center->setSpacing(30);
    lay_center->addLayout(lay_logo);
    lay_center->addLayout(lay_btn);

    QHBoxLayout *lay_hcenter = new QHBoxLayout;
    lay_hcenter->setContentsMargins(0, 0, 0, 0);
    lay_hcenter->addStretch();
    lay_hcenter->addLayout(lay_center);
    lay_hcenter->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(m_background);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch();
    layout->addLayout(lay_hcenter);
    layout->addStretch();
}

void HomePage::gSettingChanged(QString key)
{
    if (key == "systemFontSize" || key == "systemFont") {
        // 字体改变
        QString fontFamily = g_gsettings->get("systemFont").toString();
        double fontSize = g_gsettings->get("systemFontSize").toDouble();
        QFont f(fontFamily);
        f.setPointSizeF(fontSize);
        m_btnOpenFile->setFont(f);
        m_btnOpenDir->setFont(f);
    }
}

void HomePage::resizeEvent(QResizeEvent *e)
{
    m_background->resize(size());
}
