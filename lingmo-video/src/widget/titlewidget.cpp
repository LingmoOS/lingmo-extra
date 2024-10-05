#include "titlewidget.h"
#include <QLabel>
#include <QVariant>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QGSettings>
#include <QDebug>

#include "global/global.h"
#include "kmenu.h"

using namespace Global;

TitleWidget::TitleWidget(QWidget *parent) : QWidget(parent)
{
    initLayout();
    initConnect();
    if (g_config->videoOutputType() == GlobalConfig::VO_OPENGL)
        setAttribute(Qt::WA_TranslucentBackground, true);
}

TitleWidget::~TitleWidget()
{

}

int TitleWidget::getMenuBtnX()
{
    return m_btnMenu->x();
}

void TitleWidget::setTitle(QString title)
{
    m_fullTitle = title;
    QFontMetrics fontWidth(labTitle->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(m_fullTitle, Qt::ElideRight, labTitle->width() < 720 ? 720 : labTitle->width());//最大宽度150像素
    labTitle->setText(show_name);

    if (show_name != m_fullTitle)
        labTitle->setToolTip(m_fullTitle);
    else
        labTitle->setToolTip("");
}

/** ************************************************************************
 * 更新最大化按钮图标
 ***************************************************************************/
void TitleWidget::updateMaxButtonStatus(bool _isMaxed)
{
    if (_isMaxed)
    {
        m_btnMaxSize->setIcon(QIcon::fromTheme("window-restore-symbolic"));
        m_btnMaxSize->setToolTip(tr("Restore"));
    }
    else
    {
        m_btnMaxSize->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        m_btnMaxSize->setToolTip(tr("Maximize"));
    }
    m_btnMaxSize->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
}

void TitleWidget::setButtonState(bool _isAvailable)
{
    m_btnMinSize->setVisible(_isAvailable);
    m_btnMaxSize->setVisible(Global::isTablet ? false : _isAvailable);
    if(!m_isHomePage && !Global::isTablet) {
        if (g_config->videoOutputType() == GlobalConfig::VO_WID)
            m_btnMiniMode->setVisible(false);
        else
            m_btnMiniMode->setVisible(_isAvailable);
    }
}

void TitleWidget::setHide()
{
    hide();
}

void TitleWidget::setShow()
{
    if(!m_isMiniMode)
        show();
}

void TitleWidget::setHomePage(bool _isHomePage)
{
    if (_isHomePage == m_isHomePage)
        return;
    m_isHomePage = _isHomePage;

    if (Global::isTablet) {
        m_btnMiniMode->setVisible(false);
        return;
    }

    if (g_config->videoOutputType() == GlobalConfig::VO_WID)
        m_btnMiniMode->setVisible(false);
    else
        m_btnMiniMode->setVisible(!_isHomePage);
}

void TitleWidget::setUIMode(bool isTabletMode)
{
    m_isTabletMode = isTabletMode;
    if (m_isTabletMode) {
        m_btnMaxSize->setVisible(false);
        m_btnMiniMode->setVisible(false);
    }
    else {
        m_btnMaxSize->setVisible(true);
        if (g_config->videoOutputType() == GlobalConfig::VO_OPENGL && !m_isHomePage) {
            m_btnMiniMode->setVisible(true);
        }
    }
	resetSize();
}

void TitleWidget::initLayout()
{
    // 标题栏没有主题适配，黑白主题都是黑底白字
    setFixedHeight(TITLE_BAR_FIXED_HEIGHT);

    menu = new KMenu;

    widget = new QWidget(this);
    widget->setObjectName("widget");
    // 渐变背景
    widget->setStyleSheet("#widget{background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(19, 19, 20, 77), stop:1 rgba(255, 255, 255, 0));}");

    QHBoxLayout *h = new QHBoxLayout(this);
    h->setContentsMargins(0,0,0,0);
    h->addWidget(widget);
    QHBoxLayout *hb = new QHBoxLayout(widget);
    hb->setSpacing(4);

    btnIcon = new QPushButton;
    btnIcon->setFlat(true);
    btnIcon->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    btnIcon->setIcon(QIcon::fromTheme("lingmo-video"));
    hb->addWidget(btnIcon);

    labTitle = new QLabel;
    labTitle->setObjectName("title");
    labTitle->setStyleSheet("#title{color:#ffffff;}");

    hb->addWidget(labTitle);
    hb->setStretchFactor(labTitle, 1);
    labTitle->setText(tr("Video Player"));
    hb->setContentsMargins(8, 0, 4, 0);

    m_btnMenu = new QPushButton;
    hb->addWidget(m_btnMenu);
    m_btnMenu->setProperty("isOptionButton", true);
    m_btnMenu->setProperty("isWindowButton", 0x1);
    m_btnMenu->setProperty("useIconHighlightEffect", 0x2);
    m_btnMenu->setFlat(true);
    m_btnMenu->setIcon(QIcon::fromTheme("open-menu-symbolic"));
    m_btnMenu->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
    m_btnMenu->setToolTip(tr("Options"));

    m_btnMiniMode = new QPushButton;
    hb->addWidget(m_btnMiniMode);
    m_btnMiniMode->setProperty("isOptionButton", true);
    m_btnMiniMode->setProperty("isWindowButton", 0x1);
    m_btnMiniMode->setProperty("useIconHighlightEffect", 0x2);
    m_btnMiniMode->setFlat(true);
    m_btnMiniMode->setIcon(QIcon::fromTheme("lingmo-mini-symbolic"));
    m_btnMiniMode->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
    m_btnMiniMode->setVisible(false);
    m_btnMiniMode->setToolTip(tr("Mini mode"));

    m_btnMinSize = new QPushButton;
    hb->addWidget(m_btnMinSize);
    m_btnMinSize->setProperty("isWindowButton", 0x1);
    m_btnMinSize->setProperty("useIconHighlightEffect", 0x2);
    m_btnMinSize->setFlat(true);
    m_btnMinSize->setIcon(QIcon::fromTheme("window-minimize-symbolic"));
    m_btnMinSize->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
    m_btnMinSize->setToolTip(tr("Minimize"));

    m_btnMaxSize = new QPushButton;
    hb->addWidget(m_btnMaxSize);
    m_btnMaxSize->setProperty("isWindowButton", 0x1);
    m_btnMaxSize->setProperty("useIconHighlightEffect", 0x2);
    m_btnMaxSize->setFlat(true);
    m_btnMaxSize->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
    m_btnMaxSize->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
    m_btnMaxSize->setToolTip(tr("Maximize"));
    if (Global::isTablet)
        m_btnMaxSize->setVisible(false);

    m_btnClose = new QPushButton;
    hb->addWidget(m_btnClose);
    m_btnClose->setProperty("isWindowButton", 0x2);
    m_btnClose->setProperty("useIconHighlightEffect", 0x8);
    m_btnClose->setFlat(true);
    m_btnClose->setIcon(QIcon::fromTheme("window-close-symbolic"));
    m_btnClose->setProperty("setIconHighlightEffectDefaultColor", QColor(Qt::white));
    m_btnClose->setToolTip(tr("Close"));

    resetFont();
    resetSize();
}

void TitleWidget::initConnect()
{
    connect(m_btnClose,       &QPushButton::clicked,  [this](){emit sigClose();});
    connect(m_btnMaxSize,     &QPushButton::clicked,  [this](){emit sigMaxSize();});
    connect(m_btnMinSize,     &QPushButton::clicked,  [this](){emit sigMiniSize();});
    if (g_config->videoOutputType() == GlobalConfig::VO_OPENGL)
        connect(m_btnMiniMode,    &QPushButton::clicked,  [this](){emit sigMiniMode();});
    connect(m_btnMenu,        &QPushButton::clicked,  [this](){emit sigShowMenu();});
    connect(g_gsettings,    &QGSettings::changed,   [this](QString key){
        if (key == "systemFontSize" || key == "systemFont")
            resetFont();
    });
}

void TitleWidget::resetFont()
{
    QString fontFamily = g_gsettings->get("systemFont").toString();
    double fontSize = g_gsettings->get("systemFontSize").toDouble();

    QFont f(fontFamily);
    f.setPointSizeF(fontSize);
    labTitle->setFont(f);

    QFontMetrics fontWidth(labTitle->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(m_fullTitle, Qt::ElideRight, labTitle->width() < 720 ? 720 : labTitle->width());//最大宽度150像素
    labTitle->setText(show_name);

    if (show_name != m_fullTitle)
        labTitle->setToolTip(m_fullTitle);
    else
        labTitle->setToolTip("");
}

void TitleWidget::resetSize()
{
    if (Global::isTablet) {
        setFixedHeight(TITLE_BAR_FIXED_HEIGHT_TBALE);
        btnIcon->setFixedSize(QSize(44, 44));
        btnIcon->setIconSize(QSize(44, 44));
        m_btnMenu->setFixedSize(QSize(48, 48));
        m_btnClose->setFixedSize(QSize(48, 48));
        m_btnMaxSize->setFixedSize(QSize(48, 48));
        m_btnMiniMode->setFixedSize(QSize(48, 48));
        m_btnMinSize->setFixedSize(QSize(48, 48));
    }
    else {
        setFixedHeight(TITLE_BAR_FIXED_HEIGHT);
        btnIcon->setFixedSize(QSize(26, 26));
        btnIcon->setIconSize(QSize(26, 26));
        m_btnMenu->setFixedSize(QSize(30, 30));
        m_btnClose->setFixedSize(QSize(30, 30));
        m_btnMaxSize->setFixedSize(QSize(30, 30));
        m_btnMiniMode->setFixedSize(QSize(30, 30));
        m_btnMinSize->setFixedSize(QSize(30, 30));
    }
}

bool TitleWidget::event(QEvent *e)
{
    return QWidget::event(e);
}

void TitleWidget::enterEvent(QEvent *e)
{
    if(m_leaveState) {
        m_leaveState = false;
        emit sigCanHide(false);
    }
    return QWidget::enterEvent(e);
}

void TitleWidget::leaveEvent(QEvent *e)
{
    m_leaveState = true;
    emit sigCanHide(true);
    return QWidget::leaveEvent(e);
}

void TitleWidget::resizeEvent(QResizeEvent *e)
{
    QFontMetrics fontWidth(labTitle->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(m_fullTitle, Qt::ElideRight, labTitle->width() < 720 ? 720 : labTitle->width());
    labTitle->setText(show_name);

    if (show_name != m_fullTitle)
        labTitle->setToolTip(m_fullTitle);
    else
        labTitle->setToolTip("");
    return QWidget::resizeEvent(e);
}

void TitleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit sigMaxSize();
}
