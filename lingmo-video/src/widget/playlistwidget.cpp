#include "playlistwidget.h"
#include "imagelabel.h"
#include "tipwidget.h"
#include "textlabel.h"
#include "kmenu.h"
#include "core/playlist.h"
#include "global/functions.h"

#include <QLabel>
#include <QDebug>
#include <QWindow>
#include <QThread>
#include <QProcess>
#include <QScrollBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QGuiApplication>
#include <QVariantAnimation>
#include <QPropertyAnimation>

#include <lingmo-log4qt.h>

MarkListItem::MarkListItem(QWidget *parent) :
    ThemeWidget(parent)
{
    if (Global::isTablet)
        setFixedWidth(WIDGET_WIDTH_TABLET);
    else
        setFixedWidth(WIDGET_WIDTH);

    initStyle();
    m_needSeekToPos = false;
    m_fileExit = true;

    connect(g_core_signal, &GlobalCoreSignal::sigFileInfoChange, this, [this](Mpv::FileInfo info) {
        m_currentPlayingFile = info.file_path;
        // 是否双击书签打开的文件，如果是双击书签打开的需要跳转到书签位置
        if (m_needSeekToPos) {
            g_user_signal->seek(m_markPos);
            m_needSeekToPos = false;
        }
    }, Qt::QueuedConnection);

    connect(g_core_signal, &GlobalCoreSignal::sigFileNotExist, this, &MarkListItem::setNotExit, Qt::QueuedConnection);

    connectThemeSetting();
}

MarkListItem::~MarkListItem()
{
    if (m_pixmapPreview)
        delete m_pixmapPreview;
    if (m_labPreview)
        delete m_labPreview;
    if (m_labFilePath)
        delete m_labFilePath;
    if (m_labDuration)
        delete m_labDuration;
    if (m_labMarkPos)
        delete m_labMarkPos;
    if (m_vlay)
        delete m_vlay;
    if (m_hlay)
        delete m_hlay;
    if (m_dp)
        delete m_dp;
    if (m_btnDelete)
        delete m_btnDelete;

    m_pixmapPreview = nullptr;
    m_labPreview = nullptr;
    m_labFilePath = nullptr;
    m_labDuration = nullptr;
    m_labMarkPos = nullptr;
    m_vlay = nullptr;
    m_hlay = nullptr;
    m_dp = nullptr;
    m_btnDelete = nullptr;
}

void MarkListItem::setData(QString path, int duration, int pos, QString desc, QString preview)
{
    m_tooltip = path;
    m_fileName = path.split("/").last();
    m_labDuration->setText(Functions::timeToStr((double)duration));
    m_labPreview->setTime(Functions::timeToStr((double)duration));
    m_labMarkPos->setText(Functions::timeToStr((double)pos));
    m_labFilePath->setText(m_fileName);
    m_markPos = pos;
    m_describe = desc;

    // 加载预览图
    QFile file_view(preview);
    if(file_view.exists())
        m_pixmapPreview = new QPixmap(preview);
    else
        m_pixmapPreview = new QPixmap(":/ico/no-preview.png");
}

void MarkListItem::setMode(ListViewMode mode)
{
    m_mode = mode;
    switch (m_mode) {
    case LIST:
        m_labFilePath->setMaximumWidth(190);
        m_labPreview->setFixedSize(0, 54);
//        m_labDuration->setFixedWidth(66);
        m_labDuration->show();
        m_hlay->setContentsMargins(8, 1, 25, 1);
        break;
    case VIEW:
        m_labFilePath->setMaximumWidth(150);
        m_labPreview->setFixedSize(80, 54);
//        m_labDuration->setFixedWidth(0);
        m_labDuration->hide();
        m_hlay->setContentsMargins(16, 10, 25, 10);
        break;
    default:
        break;
    }
    updateNameShow();
    m_labPreview->setPixmap((*m_pixmapPreview).scaled(80, 54));
}

void MarkListItem::setEnter()
{
    m_labDuration->hide();
    m_btnDelete->show();
}

void MarkListItem::setLeave()
{
    m_btnDelete->hide();
    if (m_mode == LIST)
        m_labDuration->show();
}

void MarkListItem::setWidgetFont(QFont f)
{
    m_labDuration->setFont(f);
    m_labFilePath->setFont(f);
    m_labMarkPos->setFont(f);

    updateNameShow();
}

void MarkListItem::setNotExit(QString file)
{
    if (file != m_tooltip) {
        return;
    }
    m_fileExit = false;
    setFontColor(QColor("#A6A6A6"));
}

void MarkListItem::setFontColor(QColor color)
{
    if (color.name() == "")
    {
        m_labFilePath->setStyleSheet("");
        m_labDuration->setStyleSheet("");
        m_labMarkPos->setStyleSheet("");
    }
    else
    {
        m_labFilePath->setStyleSheet(QString("color:").append(color.name()));
        m_labDuration->setStyleSheet(QString("color:").append(color.name()));
        m_labMarkPos->setStyleSheet(QString("color:").append(color.name()));
    }
}

void MarkListItem::setBlackTheme()
{
    m_btnDelete->setStyleSheet("QPushButton:!hover{background-color:rgba(255,255,255,0);border-image:url(:/ico/cha-h.png);}"
                               "QPushButton:hover{background-color:rgba(255,255,255,0);border-image:url(:/ico/cha-w.png);}"
                               "QPushButton:pressed{background-color:rgba(255,255,255,0);border-image:url(:/ico/cha-w.png);}");
}

void MarkListItem::setDefaultTheme()
{
    m_btnDelete->setStyleSheet("QPushButton:!hover{background-color:rgba(255,255,255,0);border-image:url(:/ico_light/cha-h.png);}"
                               "QPushButton:hover{background-color:rgba(255,255,255,0);border-image:url(:/ico_light/cha-w.png);}"
                               "QPushButton:pressed{background-color:rgba(255,255,255,0);border-image:url(:/ico_light/cha-w.png);}");
}

void MarkListItem::initStyle()
{
    m_labFilePath   = new QLabel;
    m_labMarkPos    = new QLabel;
    m_labDuration   = new QLabel;
    m_labPreview    = new ImageLabel;
    m_btnDelete     = new QPushButton;

    m_dp = new QWidget;

    connect(m_btnDelete, &QPushButton::clicked, [this](){
        emit sigDeleteMark(m_tooltip, m_markPos);
    });

    m_btnDelete->setFixedSize(16, 16);
    m_btnDelete->setCursor(Qt::PointingHandCursor);

    m_vlay = new QVBoxLayout(m_dp);
    m_vlay->setSpacing(4);
    m_vlay->setContentsMargins(0, 0, 0, 0);
    m_vlay->addStretch();
    m_vlay->addWidget(m_labFilePath);
    m_vlay->addWidget(m_labMarkPos);
    m_vlay->addStretch();

    m_hlay = new QHBoxLayout(this);
    m_hlay->setContentsMargins(16, 10, 30, 10);

    m_labDuration->setContentsMargins(0, 0, 5, 0);
    m_hlay->addWidget(m_labPreview);
    m_hlay->addWidget(m_dp);
    m_hlay->addStretch();
    m_hlay->addWidget(m_labDuration);
    m_hlay->addWidget(m_btnDelete);
    m_btnDelete->hide();
}

void MarkListItem::updateNameShow()
{
    QFontMetrics fontWidth(m_labFilePath->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(m_fileName, Qt::ElideRight, m_labFilePath->maximumWidth());//最大宽度150像素
    m_labFilePath->setText(show_name);
}

void MarkListItem::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        // 右击之后需要弹出菜单，并且告诉谁被点击的
        emit sigRightBtnPressed(m_markPos);
    }
}

void MarkListItem::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (!m_fileExit) {
            if (isWayland) {
                QMessageBox::information(parentWidget(), tr("Video Player"), tr("File not exist!"), QMessageBox::Ok);
            }
            else {
                TipWidget::showTip(tr("File not exist!"), 1000, QSize(150, 40), parentWidget(), TipWidget::WARNING);
            }
            return;
        }

        // 播放并跳转
        emit sigPlayAndSeek(m_tooltip, m_markPos);
        m_needSeekToPos = true;
    }
}

void MarkListItem::enterEvent(QEvent *e)
{
    if (m_labDuration)
        m_labDuration->hide();
    if (m_btnDelete)
        m_btnDelete->show();
    return ThemeWidget::enterEvent(e);
}

void MarkListItem::leaveEvent(QEvent *e)
{
    if (m_labDuration && m_mode == LIST)
        m_labDuration->show();
    if (m_btnDelete)
        m_btnDelete->hide();
    return ThemeWidget::leaveEvent(e);
}

bool MarkListItem::event(QEvent *e)
{
    if (e->type() == QEvent::ContextMenu) {
        e->accept();
        return true;
    }
    return ThemeWidget::event(e);
}

PlayListItem::PlayListItem(QWidget *parent) :
    ThemeWidget(parent)
{
    if (Global::isTablet)
        setFixedWidth(WIDGET_WIDTH_TABLET);
    else
        setFixedWidth(WIDGET_WIDTH);
    m_isSelected = false;
    m_isExist = true;

    initStyle();
    initAnimation();

    connectThemeSetting();
}

PlayListItem::~PlayListItem()
{
    delete m_labIcon;
    delete m_labName;
    delete m_labPreview;
    delete m_labSpacing;
    delete m_labDuration;
    delete m_btnDelete;
    delete m_pixmapPreview;
    delete layout();
}

void PlayListItem::setSelected(bool s)
{
    m_isSelected = s;
    if (m_isSelected)
    {
        // 如果设置为选中说明文件存在
        m_isExist = true;
        // 如果是预览模式的话不需要显示三角形图标
        if(m_mode == LIST)
            m_labIcon->show();
        setFontColor(m_highlightColor);
    }
    else
    {
        m_labIcon->hide();

        if (FOLLOW_SYS_THEME) {
            if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                setDefaultTheme();
            else
                setBlackTheme();
        }
        else {
            setBlackTheme();
        }
    }
}

void PlayListItem::setData(QString file, int duration, int _lastTime, QString viewPath)
{
    m_tip = file;
    // 判断文件是否存在
    QFileInfo t_fi(file);
    if(!t_fi.exists())
        slotNotExit(file);

    // 判断预览文件是否存在，不存在设置默认
    m_viewPath = viewPath;
    QFileInfo t_fi_view(viewPath);
    if(t_fi_view.exists() && t_fi_view.size() != 0)
        m_pixmapPreview = new QPixmap(viewPath);
    else
        m_pixmapPreview = new QPixmap(":/ico/no-preview.png");
    m_showName = file.split("/").last();
#if 0
    QFontMetrics fontWidth(m_labName->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(m_showName, Qt::ElideRight, m_labName->width());//最大宽度150像素
    m_labName->setText(show_name);
#endif
    m_labName->setText(m_showName);
    m_labName->setToolTip(m_showName);
    m_labDuration->setText(Functions::timeToStr((double)duration));
    m_labPreview->setTime(Functions::timeToStr((double)duration));

    connect(g_core_signal, &GlobalCoreSignal::sigFileNotExist, this, &PlayListItem::slotNotExit, Qt::QueuedConnection);
}

void PlayListItem::setDuration(int duration)
{
    m_labDuration->setText(Functions::timeToStr((double)duration));
    m_labPreview->setTime(Functions::timeToStr((double)duration));
}

void PlayListItem::setEnter()
{
    m_btnDelete->show();
    m_labDuration->hide();
}

void PlayListItem::setLeave()
{
    m_btnDelete->hide();
    if (m_mode == LIST)
        m_labDuration->show();
}

void PlayListItem::toBig()
{
    if(m_mode == VIEW)
        return;
    m_mode = VIEW;
    m_labSpacing->show();

#if 0
    m_anmToSmall->stop();
    m_anmToBig->start();
#else
    setMode(VIEW);
#endif

    m_labIcon->hide();
}

void PlayListItem::toSmall()
{
    if(m_mode == LIST)
        return;
    m_mode = LIST;
    m_labSpacing->hide();
#if 0
    m_anmToBig->stop();
    m_anmToSmall->start();
#else
    setMode(LIST);
    if(m_isSelected)
        m_labIcon->show();
#endif
}

void PlayListItem::reloadView()
{
    QFileInfo t_fi_view(m_viewPath);
    if(t_fi_view.exists() && t_fi_view.size() != 0)
        m_pixmapPreview = new QPixmap(m_viewPath);
    else
        m_pixmapPreview = new QPixmap(":/ico/no-preview.png");
    m_labPreview->setPixmap((*m_pixmapPreview).scaled(80, 54));
}

void PlayListItem::setMode(ListViewMode mode)
{
    m_mode = mode;
    switch (m_mode) {
    case LIST:
        m_labName->setMaximumWidth(190);
        m_labPreview->setFixedSize(0, 0);
//        m_labDuration->setFixedSize(66, 20);
        m_labDuration->show();
        m_labSpacing->hide();
        break;
    case VIEW:
        m_labName->setMaximumWidth(146);
        m_labPreview->setFixedSize(80, 54);
//        m_labDuration->setFixedSize(0, 20);
        m_labDuration->hide();
        m_labSpacing->show();
        break;
    default:
        break;
    }
    updateNameShow();
    m_labName->updateTextShow();
    m_labPreview->setPixmap((*m_pixmapPreview).scaled(80, 54));
    emit sigValChange(m_labPreview->height() < 20 ? 20 : m_labPreview->height());
}

void PlayListItem::setBlackTheme()
{
    if(m_isSelected)
        return;
    m_btnDelete->setStyleSheet("QPushButton:!hover{background-color:rgba(255,255,255,0);border-image:url(:/ico/cha-h.png);}"
                               "QPushButton:hover{background-color:rgba(255,255,255,0);border-image:url(:/ico/cha-w.png);}"
                               "QPushButton:pressed{background-color:rgba(255,255,255,0);border-image:url(:/ico/cha-w.png);}");
    if(m_isExist)
    {
        if(m_labName)
            m_labName->setStyleSheet("");
        if(m_labDuration)
            m_labDuration->setStyleSheet("");
    }
}

void PlayListItem::setDefaultTheme()
{
    if(m_isSelected)
        return;
    m_btnDelete->setStyleSheet("QPushButton:!hover{background-color:rgba(255,255,255,0);border-image:url(:/ico_light/cha-h.png);}"
                               "QPushButton:hover{background-color:rgba(255,255,255,0);border-image:url(:/ico_light/cha-w.png);}"
                               "QPushButton:pressed{background-color:rgba(255,255,255,0);border-image:url(:/ico_light/cha-w.png);}");
    if (m_isExist)
    {
        if(m_labName)
            m_labName->setStyleSheet("");
        if(m_labDuration)
            m_labDuration->setStyleSheet("");
    }
}

void PlayListItem::updateHightLightColor()
{
    printf("playlist item update \n");
    if (m_isSelected)
    {
        setFontColor(m_highlightColor);
    }
    QString theme_color = g_gsettings->get("themeColor").toString();
    m_labIcon->setPixmap(QPixmap(QString(":/ico/play-%1.png").arg(theme_color)).scaled(m_labIcon->size()));
}

void PlayListItem::setWidgetFont(const QFont &f)
{
    m_labName->setFont(f);
    m_labDuration->setFont(f);

    updateNameShow();
}

void PlayListItem::slotNotExit(QString file)
{
    if(file != m_tip)
        return;
    // 打开的文件不存在 文件名置灰
    m_isExist = false;
    setFontColor(QColor("#A6A6A6"));
}

void PlayListItem::initStyle()
{
    setAttribute(Qt::WA_TranslucentBackground);

    m_labIcon = new QLabel;
    m_labIcon->setFixedSize(16, 16);
    QString theme_color = g_gsettings->get("themeColor").toString();
    m_labIcon->setPixmap(QPixmap(QString(":/ico/play-%1.png").arg(theme_color)).scaled(m_labIcon->size()));

    // 播放列表预览框
    m_labPreview = new ImageLabel;
    m_labPreview->setFixedSize(0,0);

    // 占位用
    m_labSpacing = new QLabel;
    m_labSpacing->setFixedWidth(10);
    m_labSpacing->setStyleSheet("background-color:rgba(1,1,1,0);");

    m_labName = new TextLabel;
    m_labName->setObjectName("LabName");
    m_labDuration = new QLabel;
    m_labDuration->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    initDeleteButton();

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(16, 5, 30, 5);
    lay->setSpacing(0);
    lay->addWidget(m_labIcon);
    lay->addWidget(m_labPreview);
    lay->addWidget(m_labSpacing);
    lay->addWidget(m_labName);
    lay->addStretch();
    lay->addWidget(m_labDuration);
    lay->addWidget(m_btnDelete);
    lay->setStretchFactor(m_labName, 1);

    m_labIcon->hide();
    m_btnDelete->hide();

}

void PlayListItem::initDeleteButton()
{
    m_btnDelete = new QPushButton;
    m_btnDelete->setFixedSize(16, 16);
    m_btnDelete->setCursor(Qt::PointingHandCursor);

    connect(m_btnDelete, &QPushButton::clicked, this, &PlayListItem::deleteItem);
}

void PlayListItem::initAnimation()
{
    m_anmToBig = new QVariantAnimation(this);
    m_anmToBig->setDuration(AnmationDuration);
    m_anmToBig->setStartValue(0);
    m_anmToBig->setEndValue(300);
    connect(m_anmToBig, &QVariantAnimation::valueChanged, [&](QVariant _value){
        double val = _value.toDouble();

        m_labName->setMaximumWidth(190-0.147*val);
        m_labPreview->setFixedSize(val*0.26667, val*0.18);
        m_labDuration->setFixedWidth(66 - val*0.22);
        updateNameShow();
        emit sigValChange(m_labPreview->height());
    });

    m_anmToSmall = new QVariantAnimation(this);
    m_anmToSmall->setDuration(AnmationDuration);
    m_anmToSmall->setStartValue(0);
    m_anmToSmall->setEndValue(300);
    connect(m_anmToSmall, &QVariantAnimation::valueChanged, [&](QVariant _value){
        double val = _value.toDouble();

        m_labName->setMaximumWidth(146+0.147*val);
        m_labPreview->setFixedSize(80-val*0.26667, 54-val*0.18);
        m_labDuration->setFixedWidth(val*0.22);
        updateNameShow();
        if(m_labPreview->height() < 20)
            emit sigValChange(20);
        else
            emit sigValChange(m_labPreview->height());
    });
    connect(m_anmToSmall, &QVariantAnimation::finished, [&](){
        // 如果是选中状态的话显示播放按钮
        if(m_isSelected)
            m_labIcon->show();
    });
}

void PlayListItem::updateNameShow()
{
    QFontMetrics fontWidth(m_labName->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(m_showName, Qt::ElideRight, m_labName->width());//最大宽度150像素
    m_labName->setText(show_name);
    m_labName->updateTextShow();
}

void PlayListItem::updateText()
{

}

void PlayListItem::setFontColor(QColor color)
{
    if(color.name() == "")
    {
        m_labName->setStyleSheet("");
        m_labDuration->setStyleSheet("");
    }
    else
    {
        m_labName->setStyleSheet(QString("#LabName{color:%0}").arg(color.name()));
        m_labDuration->setStyleSheet(QString("color:").append(color.name()));
    }
}

void PlayListItem::deleteItem()
{
    if (m_isSelected)
    {
        // 正在播放的是本条 需要关闭
        g_user_signal->play();
        g_user_signal->stop();
    }
    // 删除条目 告诉别人删除的绝对路径就行
    g_user_signal->deleteListItem(m_tip);
}

//void PlayListItem::mousePressEvent(QMouseEvent *e)
//{
//    if(e->button() == Qt::RightButton)
//    {
//        // 右击之后需要弹出菜单，并且告诉谁被点击的
//        emit sigRightBPressed(m_tip);
//    }
//    e->accept();
//}

void PlayListItem::mouseDoubleClickEvent(QMouseEvent *e)
{
    // 判断文件是否存在，每次点击都去判断一下，这样更准确，只有在双击的时候才会去刷新状态，不会主动刷新
    QFileInfo fi(m_tip);
    if (fi.exists()) {
        emit sigExistsStateChange(m_tip, true);
        m_isExist = true;
    }
    g_user_signal->stop();
    g_user_signal->play();
    g_user_signal->open(m_tip, 0);
}

bool PlayListItem::event(QEvent *e)
{
    if (e->type() == QEvent::ContextMenu) {
        emit sigRightBPressed(m_tip);
        e->accept();
        return true;
    }
    return ThemeWidget::event(e);
}


PlayListWidget::PlayListWidget(QWidget *parent) :
    ThemeWidget(parent)
{
    initLayout();
    initMenu();
    initPlayList();
    initAnimation();
    initGlobalSig();

    m_isShow = false;
    m_isMouseEnter = false;

    connectThemeSetting();
}

PlayListWidget::~PlayListWidget()
{

}

void PlayListWidget::setBlackTheme()
{
    m_isBlackTheme = true;
    m_icoDir = "ico";
    m_theme = STYLE_LINGMO_BLACK;
    m_right->setStyleSheet("#m_right{background-color: rgba(31, 32, 34, 240);"
                             "border-top-left-radius:12px;"
                             "border-bottom-left-radius:12px;}");

    m_playListWidget->setStyleSheet("QListWidget{background-color: rgba(255, 255, 255, 0);}"
                                    "QListWidget::item::hover{background-color:rgba(255,255,255,20);}"
                                    "QListWidget::item::selected{background-color:rgba(0,0,0,0);}");

    m_markListWidget->setStyleSheet("QListWidget{background-color: rgba(255, 255, 255, 0);}"
                                    "QListWidget::item::hover{background-color:rgba(255,255,255,20);}"
                                    "QListWidget::item::selected{background-color:rgba(0,0,0,0);}");


    QString themeColor = g_gsettings->get("themeColor").toString();
    // 侧边箭头样式
    if (m_isShow) {
        m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left1-d.png);}"
                                    "QPushButton:hover{border-image: url(:/%1/icon-left1-%2-h.png);}"
                                    "QPushButton:pressed{border-image: url(:/%1/icon-left1-%2-h.png);}")
                                    .arg(m_icoDir).arg(themeColor));
    }
    else {
        m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left2-d.png);}"
                                    "QPushButton:hover{border-image: url(:/%1/icon-left2-%2-h.png);}"
                                    "QPushButton:pressed{border-image: url(:/%1/icon-left2-%2-h.png);}")
                                    .arg(m_icoDir).arg(themeColor));
    }

    updateHightLightColor();

    m_homePic->setStyleSheet("border-image:url(:/ico/list-home-new.png);");

    updateOrderIcon();
    updateViewIcon();
    updateAddIcon();
    updateDeleteIcon();
}

void PlayListWidget::setDefaultTheme()
{
    m_isBlackTheme = false;
    m_icoDir = "ico_light";
    m_theme = STYLE_LINGMO_LIGHT;
    m_right->setStyleSheet("#m_right{background-color: rgba(249, 249, 249, 220);"
                             "border-top-left-radius:12px;"
                             "border-bottom-left-radius:12px;}");

    m_playListWidget->setStyleSheet("QListWidget{background-color: rgba(255, 255, 255, 0);color:#262626;}"
                                    "QListWidget::item::hover{background-color:rgba(0,0,0,20);color:#262626;}"
                                    "QListWidget::item::selected{background-color:rgba(0,0,0,0);color:#262626;}");

    m_markListWidget->setStyleSheet("QListWidget{background-color: rgba(255, 255, 255, 0);color:#262626;}"
                                    "QListWidget::item::hover{background-color:rgba(0,0,0,20);color:#262626;}"
                                    "QListWidget::item::selected{background-color:rgba(0,0,0,0);color:#262626;}");


    QString themeColor = g_gsettings->get("themeColor").toString();
    // 侧边箭头样式
    if (m_isShow) {
        m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left1-d.png);}"
                                    "QPushButton:hover{border-image: url(:/%1/icon-left1-%2-h.png);}"
                                    "QPushButton:pressed{border-image: url(:/%1/icon-left1-%2-h.png);}")
                                    .arg(m_icoDir).arg(themeColor));
    }
    else {
        m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left2-d.png);}"
                                    "QPushButton:hover{border-image: url(:/%1/icon-left2-%2-h.png);}"
                                    "QPushButton:pressed{border-image: url(:/%1/icon-left2-%2-h.png);}")
                                    .arg(m_icoDir).arg(themeColor));
    }

    updateHightLightColor();

    m_homePic->setStyleSheet("border-image:url(:/ico/list-home.png);");

    updateOrderIcon();
    updateViewIcon();
    updateAddIcon();
    updateDeleteIcon();
}

void PlayListWidget::showItemMenu(QString file)
{
    m_selectedFile = file;
    if (QGuiApplication::focusWindow())
        m_itemMenu->exec(QPoint(QCursor::pos(QGuiApplication::focusWindow()->screen()) + QPoint(1,1)));
}

void PlayListWidget::addItem(PlayItem item)
{
    std::lock_guard<std::mutex> lg(m_itemAddMux);
    PlayListItem *itemWidget  = new PlayListItem;
    connect(itemWidget, &PlayListItem::sigExistsStateChange, this, [this](QString path, bool exist){
        if (exist)
            m_playList->setAvaliable(path);
        else
            m_playList->setNotAvaliable(path);
    });
    connect(itemWidget, &PlayListItem::sigRightBPressed, this, &PlayListWidget::showItemMenu, Qt::QueuedConnection);
    QListWidgetItem *listItem = new QListWidgetItem;
    switch (m_mode) {
    case LIST:
        listItem->setSizeHint(QSize(314, 40));
        break;
    case VIEW:
        listItem->setSizeHint(QSize(314, 74));
        break;
    default:
        break;
    }

    m_playListWidget->addItem(listItem);
    m_playListWidget->setItemWidget(listItem, itemWidget);
    m_itemMap[item.m_filePath] = listItem;

    itemWidget->setData(item.m_filePath, item.m_duration, item.m_lastStopTime, item.m_previewPath);

    // 样式播放列表视图改变
    connect(itemWidget, &PlayListItem::sigValChange, [this, listItem](int _val){
        listItem->setSizeHint(QSize(m_playListWidget->width(), _val+20));
    });
    itemWidget->setMode(m_mode);

    itemWidget->setObjectName(item.m_filePath);
    itemWidget->installEventFilter(this);
    if (m_titleList->currentRow() == 0 && m_listStack->currentIndex() == 2) {
        m_listStack->setCurrentIndex(0);
    }
}

void PlayListWidget::deleteItem(QString file)
{
    if(m_itemMap.find(file) != m_itemMap.end())
    {
        m_playListWidget->itemWidget(m_itemMap[file])->removeEventFilter(this);
        delete m_playListWidget->itemWidget(m_itemMap[file]);
        m_playListWidget->removeItemWidget(m_itemMap[file]);
        if(m_itemMap[file])
            delete m_itemMap[file];
        m_itemMap.remove(file);
    }
    if (m_itemMap.size() == 0 && m_listStack->currentIndex() == 0 && m_titleList->currentRow() == 0) {
        m_listStack->setCurrentIndex(2);
    }
}

void PlayListWidget::clearList()
{
    // 如果列表为空就不要弹出对话框了
    if(m_playList->getSize() == 0)
        return;
    QMessageBox::StandardButton standar_button;
    QWidget* parent_widget = nullptr;
    if (isWayland)
        parent_widget = parentWidget();

    QMessageBox box(QMessageBox::Question,
                    tr("Video Player"),
                    tr("Are you sure you want to clear the list?"),
                    QMessageBox::Yes | QMessageBox::No,
                    parent_widget,
                    Qt::Dialog);

    if (isWayland)
        kdk::LingmoStyleHelper::self()->removeHeader(&box);
    box.show();
    box.move(parentWidget()->geometry().center() - box.geometry().center());
    standar_button = (QMessageBox::StandardButton)box.exec();

    if (standar_button == QMessageBox::Yes) {
        if(g_playstate > 0) {
            QMessageBox box1(QMessageBox::Question,
                            tr("Video Player"),
                            tr("The file being played will be stopped."),
                            QMessageBox::Yes | QMessageBox::No,
                            parent_widget);

            if (isWayland)
                kdk::LingmoStyleHelper::self()->removeHeader(&box1);
            box1.show();
            box1.move(parentWidget()->geometry().center() - box1.geometry().center());
            standar_button = (QMessageBox::StandardButton)box1.exec();
        }
        if (standar_button == QMessageBox::Yes) {
            m_playList->clearList();
            if (m_titleList->currentRow() == 0) {
                m_listStack->setCurrentIndex(2);
            }
        }
    }
}

void PlayListWidget::updateMarks(QVector<MarkItem> markvec)
{
//    clearMark();
    for (MarkItem mark : markvec) {
        if (m_markMap.find(mark.m_filePath) != m_markMap.end()) {
            if (m_markMap[mark.m_filePath].find(mark.m_markPos) == m_markMap[mark.m_filePath].end()) {
                // 需要添加
                MarkListItem *item = new MarkListItem;
                connect(item, &MarkListItem::sigDeleteMark, this, &PlayListWidget::deleteMark);
                connect(item, &MarkListItem::sigPlayAndSeek, [this](QString file_path, int pos) {
                    if (file_path == m_currentPlayingFile) {
                        g_user_signal->seek(pos);
                    }
                    else {
                        g_user_signal->stop();
                        g_user_signal->play();
                        g_user_signal->open(file_path, pos);
                    }
                });
                item->setData(mark.m_filePath, mark.m_duration, mark.m_markPos, mark.m_describe, mark.m_previewPath);
                item->setMode(m_mode);
                if (!mark.m_fileExit) {
                    item->setNotExit(mark.m_filePath);
                }

                QListWidgetItem *widgetItem = new QListWidgetItem;
                switch (m_mode) {
                case LIST:
                    widgetItem->setSizeHint(QSize(m_markListWidget->width(), 56));
                    break;
                case VIEW:
                    widgetItem->setSizeHint(QSize(m_markListWidget->width(), 74));
                    break;
                default:
                    break;
                }

                m_markMap[mark.m_filePath][mark.m_markPos] = widgetItem;
                int insertIndex = 0;
                bool insertFlag = false;
                bool insertSection = false;
                for (;;) {
                    QListWidgetItem *it = m_markListWidget->item(insertIndex);
                    if (!it) {
                        m_markListWidget->insertItem(insertIndex, widgetItem);
                        m_markListWidget->setItemWidget(widgetItem, item);
                        break;
                    }
                    if (dynamic_cast<MarkListItem*> (m_markListWidget->itemWidget(it))->getFilePath() == mark.m_filePath) {
                        insertSection = true;
                        if (dynamic_cast<MarkListItem*> (m_markListWidget->itemWidget(it))->getPos() > mark.m_markPos) {
                            // 插入
                            m_markListWidget->insertItem(insertIndex, widgetItem);
                            m_markListWidget->setItemWidget(widgetItem, item);
                            insertFlag = true;
                            break;
                        }
                    }
                    else if (insertSection && !insertFlag) {
                        m_markListWidget->insertItem(insertIndex, widgetItem);
                        m_markListWidget->setItemWidget(widgetItem, item);
                        break;
                    }
                    insertIndex++;
                }
            }
        }
        else {
            // 需要添加
            MarkListItem *item = new MarkListItem;
            connect(item, &MarkListItem::sigDeleteMark, this, &PlayListWidget::deleteMark);
            connect(item, &MarkListItem::sigPlayAndSeek, [this](QString file_path, int pos) {
                if (file_path == m_currentPlayingFile) {
                    g_user_signal->seek(pos);
                }
                else {
                    g_user_signal->stop();
                    g_user_signal->play();
                    g_user_signal->open(file_path, pos);
                }
            });
            item->setData(mark.m_filePath, mark.m_duration, mark.m_markPos, mark.m_describe, mark.m_previewPath);
            item->setMode(m_mode);
            if (!mark.m_fileExit) {
                item->setNotExit(mark.m_filePath);
            }

            QListWidgetItem *widgetItem = new QListWidgetItem;
            switch (m_mode) {
            case LIST:
                widgetItem->setSizeHint(QSize(m_markListWidget->width(), 56));
                break;
            case VIEW:
                widgetItem->setSizeHint(QSize(m_markListWidget->width(), 74));
                break;
            default:
                break;
            }

            m_markMap[mark.m_filePath][mark.m_markPos] = widgetItem;
            m_markListWidget->insertItem(0, widgetItem);
            m_markListWidget->setItemWidget(widgetItem, item);
        }
        continue;

#if 0
        MarkListItem *item = new MarkListItem;
        connect(item, &MarkListItem::sigDeleteMark, this, &PlayListWidget::deleteMark);
        connect(item, &MarkListItem::sigPlayAndSeek, [this](QString file_path, int pos) {
            if (file_path == m_currentPlayingFile) {
                g_user_signal->seek(pos);
            }
            else {
                g_user_signal->stop();
                g_user_signal->play();
                g_user_signal->open(file_path, pos);
            }
        });
        item->setData(mark.m_filePath, mark.m_duration, mark.m_markPos, mark.m_describe, mark.m_previewPath);
        item->setMode(m_mode);
        if (!mark.m_fileExit) {
            item->setNotExit(mark.m_filePath);
        }

        QListWidgetItem *widgetItem = new QListWidgetItem;
        switch (m_mode) {
        case LIST:
            widgetItem->setSizeHint(QSize(m_markListWidget->width(), 56));
            break;
        case VIEW:
            widgetItem->setSizeHint(QSize(m_markListWidget->width(), 74));
            break;
        default:
            break;
        }

        m_markListWidget->addItem(widgetItem);
        m_markListWidget->setItemWidget(widgetItem, item);

        if (!mark.m_fileExit) {
            item->setNotExit(mark.m_filePath);
        }
#endif
    }
}

void PlayListWidget::deleteMark(QString filepath, int pos)
{
    for (int i=0; i<m_markListWidget->count(); i++) {
        QListWidgetItem *item = m_markListWidget->item(i);
        if (((MarkListItem*)m_markListWidget->itemWidget(item))->getPos() == pos &&
                ((MarkListItem*)m_markListWidget->itemWidget(item))->getFilePath() == filepath) {
            // 删除数据库数据，应该交给playlist处理，不要在界面处理
            m_playList->deleteMark(((MarkListItem*)m_markListWidget->itemWidget(item))->getFilePath(), pos);
            // 删除界面
            delete m_markListWidget->itemWidget(item);
            // 删除 item
            m_markListWidget->removeItemWidget(item);
            delete item;
        }
    }
}

void PlayListWidget::clearMark()
{
    while (m_markListWidget->count()) {
        QListWidgetItem *item = m_markListWidget->item(0);
        // 隐藏就不会出发 leaveEvent 了，不然可能会导致崩溃
        m_markListWidget->itemWidget(item)->hide();
        delete m_markListWidget->itemWidget(item);
        m_markListWidget->removeItemWidget(item);
        delete item;
    }
    m_markMap.clear();
}

void PlayListWidget::reloadView(QString file)
{
    std::lock_guard<std::mutex> lg(m_itemAddMux);
    QMap<QString, QListWidgetItem*> tmp_map(m_itemMap);
    if(tmp_map.find(file) != tmp_map.end()) {
        ((PlayListItem*)m_playListWidget->itemWidget(tmp_map[file]))->reloadView();
    }
}

void PlayListWidget::resetDuration(QString file, int duration)
{
    std::lock_guard<std::mutex> lg(m_itemAddMux);
    QMap<QString, QListWidgetItem*> tmp_map(m_itemMap);
    if(tmp_map.find(file) != tmp_map.end()) {
        ((PlayListItem*)m_playListWidget->itemWidget(tmp_map[file]))->setDuration(duration);
    }
}

void PlayListWidget::slotShow()
{
    m_canHideLeftButton = false;
    if (m_titleList->currentRow() == 0)
        m_listStack->setCurrentIndex(m_itemMap.size() == 0 ? 2 : 0);

    if(pos().x() == ((QWidget*)parent())->width() - width())
        return;

    m_hideAnm->stop();
    m_showAnm->setStartValue(geometry());
    m_showAnm->setEndValue(QRect(((QWidget*)parent())->width() - width(), 0, width(), height()));
    m_showAnm->start();
}

void PlayListWidget::slotHide()
{
    m_canHideLeftButton = true;
    if(pos().x() == ((QWidget*)parent())->width() - 16)
        return;
    m_showAnm->stop();
    m_hideAnm->setStartValue(geometry());
    m_hideAnm->setEndValue(QRect(((QWidget*)parent())->width() - 16, 0, width(), height()));
    m_hideAnm->start();
    // 隐藏的时候把所有 item 中 x 都隐藏了
    for(QListWidgetItem* t_item : m_itemMap)
        ((PlayListItem*)m_playListWidget->itemWidget(t_item))->setLeave();
}

void PlayListWidget::setShowButton(bool show)
{
    if (show) {
        this->show();
    }
    else {
        if (m_canHideLeftButton) {
            hide();
        }
    }
}

void PlayListWidget::updateShowIcon()
{
    m_showAnm->stop();
    m_hideAnm->stop();

    m_isShow = true;
    QString themeColor = g_gsettings->get("themeColor").toString();
    m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left2-d.png);}"
                                "QPushButton:hover{border-image: url(:/%1/icon-left2-%2-h.png);}"
                                "QPushButton:pressed{border-image: url(:/%1/icon-left2-%2-h.png);}")
                                .arg(m_icoDir).arg(themeColor));
    m_leftButton->show();
}

void PlayListWidget::updateHideIcon()
{
    m_showAnm->stop();
    m_hideAnm->stop();

    m_isShow = false;
    m_canHideLeftButton = true;
    QString themeColor = g_gsettings->get("themeColor").toString();
    m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left1-d.png);}"
                                "QPushButton:hover{border-image: url(:/%1/icon-left1-%2-h.png);}"
                                "QPushButton:pressed{border-image: url(:/%1/icon-left1-%2-h.png);}")
                                .arg(m_icoDir).arg(themeColor));
    // 列表隐藏之后需要显示标题栏和控制栏
    g_user_signal->hideBar(false);
}

void PlayListWidget::delAvailableIndex(QString file)
{
    TipWidget::showTip(tr("File not exist!"), 1000, QSize(150, 40), parentWidget(), TipWidget::WARNING);
    m_playList->setNotAvaliable(file);
    // 不存在的话播放下一首
    g_user_signal->playNext(false);
}

void PlayListWidget::changeListViewMode()
{
    if(m_mode == LIST)
    {
        m_mode = VIEW;
        m_viewButton->setToolTip(tr("Preview view"));
        int count = m_playListWidget->count();
        for(int i=0; i<count; i++)
            ((PlayListItem*)m_playListWidget->itemWidget(m_playListWidget->item(i)))->toBig();
        count = m_markListWidget->count();
        for (int i=0; i<count; i++) {
            ((MarkListItem*)m_markListWidget->itemWidget(m_markListWidget->item(i)))->setMode(VIEW);
            m_markListWidget->item(i)->setSizeHint(QSize(m_markListWidget->width(), 74));
        }
    }
    else
    {
        m_mode = LIST;
        m_viewButton->setToolTip(tr("List view"));
        int count = m_playListWidget->count();
        for(int i=0; i<count; i++)
            ((PlayListItem*)m_playListWidget->itemWidget(m_playListWidget->item(i)))->toSmall();
        count = m_markListWidget->count();
        for (int i=0; i<count; i++) {
            ((MarkListItem*)m_markListWidget->itemWidget(m_markListWidget->item(i)))->setMode(LIST);
            m_markListWidget->item(i)->setSizeHint(QSize(m_markListWidget->width(), 56));
        }
    }
    g_settings->setValue("General/list_view", (int)m_mode);
    updateViewIcon();
}

void PlayListWidget::setPlayIndex(int index)
{
    // 其他都设置为非选中状态
    selectNone();
    if (index >= 0 && index < m_itemMap.size())
        ((PlayListItem*)m_playListWidget->itemWidget(m_playListWidget->item(index)))->setSelected(true);
}

void PlayListWidget::selectNone()
{
    for (auto item:m_itemMap)
        ((PlayListItem*)m_playListWidget->itemWidget(item))->setSelected(false);
}

void PlayListWidget::gSettingChanged(QString key)
{
    if (key == "systemFontSize" || key == "systemFont") {
        // 字体改变
        QString fontFamily = g_gsettings->get("systemFont").toString();
        double fontSize = g_gsettings->get("systemFontSize").toDouble();
        QFont f(fontFamily);
        f.setPointSizeF(fontSize);
        m_homeText->setFont(f);
        for (auto item : m_itemMap) {
            ((PlayListItem*)m_playListWidget->itemWidget(item))->setWidgetFont(f);
        }
        for (int i=0; i<m_markListWidget->count(); i++) {
            ((MarkListItem*)m_markListWidget->itemWidget(m_markListWidget->item(i)))->setWidgetFont(f);
        }

        m_titleList->setFont(f);
        for(int i=0; i<m_titleList->count(); i++)
        {
            QListWidgetItem *it = m_titleList->item(i);
            it->setSizeHint(QSize(45 * fontSize / 10, 24));
        }

        f.setPointSizeF(fontSize - 1);
        m_titleList->setFont(f);

        m_lineDock->setFixedWidth(90 * fontSize / 10);
        m_line->move(m_lineDock->width() / 4 - m_line->width() / 2 + m_titleList->currentRow() * (m_lineDock->width() / 2), 0);
    }
}

void PlayListWidget::initLayout()
{
    {
        // 初始化左边
        m_left = new QWidget;
        m_left->setFixedWidth(16);

        m_leftButton = new QPushButton;
        m_leftButton->setCursor(Qt::PointingHandCursor);
        m_leftButton->setFixedSize(16, 80);

        QVBoxLayout *ll = new QVBoxLayout(m_left);
        ll->setContentsMargins(0,0,0,0);
        ll->addWidget(m_leftButton);
    }

    {
        // 初始化右边
        m_right = new QWidget;
        m_right->setObjectName("m_right");

        m_rightTop = new QWidget;
        m_rightTitle = new QWidget;
        m_lineDock = new QWidget;
        double fontSize = g_gsettings->get("systemFontSize").toDouble();
        m_lineDock->setFixedWidth(90 * fontSize / 10);

        m_line = new QWidget(m_lineDock);
//        m_line->setStyleSheet("background-color: rgb(55, 144, 250);");

        m_titleList = new QListWidget;
        m_titleList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_titleList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        m_orderButton = new QPushButton;
        m_viewButton = new QPushButton;
        m_addButton = new QPushButton;
        m_deleteButton = new QPushButton;
        m_listStack = new QStackedWidget;

        m_rightTitle->setMinimumWidth(40);
        QVBoxLayout *lay1 = new QVBoxLayout(m_rightTitle);
        lay1->setSpacing(0);
        lay1->setContentsMargins(0, 4, 0, 0);
        lay1->addWidget(m_titleList);
        lay1->addWidget(m_lineDock);

        QHBoxLayout *lay2 = new QHBoxLayout(m_rightTop);
        lay2->setSpacing(2);
        lay2->setContentsMargins(6, 4, 10, 0);
        lay2->addWidget(m_rightTitle);
        lay2->addStretch();
        lay2->addWidget(m_orderButton);
        lay2->addWidget(m_viewButton);
        lay2->addWidget(m_addButton);
        lay2->addWidget(m_deleteButton);

        QVBoxLayout *lay3 = new QVBoxLayout(m_right);
        lay3->setSpacing(0);
        lay3->setContentsMargins(0, 0, 0, 0);
        lay3->addWidget(m_rightTop);
        lay3->addWidget(m_listStack);

        m_playListWidget = new QListWidget;
        m_markListWidget = new QListWidget;
//        m_playListWidget->setFixedWidth(width() - 16);
//        m_markListWidget->setFixedWidth(width() - 16);

        m_listStack->addWidget(m_playListWidget);
        m_listStack->addWidget(m_markListWidget);
    }

    QHBoxLayout *lay_out = new QHBoxLayout(this);
    lay_out->setSpacing(0);
    lay_out->setContentsMargins(0, 0, 0, 0);
    lay_out->addWidget(m_left);
    lay_out->addWidget(m_right);

    {
        // 设置展开按钮属性和样式
        m_leftButton->setCursor(Qt::PointingHandCursor);
        m_leftButton->setFixedSize(16, 80);
        m_playListWidget->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
        m_markListWidget->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
        initConnection();

        m_orderButton->setCursor(Qt::PointingHandCursor);

        // 切换列表视图按钮
        m_viewButton->setCursor(Qt::PointingHandCursor);
        // 播放列表视图模式
        if(!g_settings->contains("General/list_view"))
            g_settings->setValue("General/list_view", 0);
        m_mode = (ListViewMode)g_settings->value("General/list_view").toInt();

        m_addButton->setCursor(Qt::PointingHandCursor);
        m_deleteButton->setCursor(Qt::PointingHandCursor);

        m_playListWidget->setSpacing(0);
        m_playListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        m_markListWidget->setSpacing(0);
        m_markListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        initDefaultWidget();

        m_titleList->setFlow(QListView::LeftToRight);
        m_titleList->addItems(QStringList() << tr("Video") << tr("Marks"));
        QString fontFamily = g_gsettings->get("systemFont").toString();
        double fontSize = g_gsettings->get("systemFontSize").toDouble();
        QFont f(fontFamily);
        f.setPointSizeF(fontSize - 1);
        m_titleList->setFont(f);
        for(int i=0; i<m_titleList->count(); i++)
        {
            QListWidgetItem *it = m_titleList->item(i);
            it->setTextAlignment(Qt::AlignCenter);
            it->setSizeHint(QSize(45 * fontSize / 10, 24));
        }
        m_titleList->setCurrentRow(0);
        m_titleList->setSpacing(0);
        m_titleList->setFixedHeight(30);

        connect(m_titleList, &QListWidget::currentRowChanged, [this](int currentRow){
            int line_pos = currentRow;
            if (currentRow == 1) {
                m_addButton->hide();
                currentRow = m_markMap.size() == 0 ? 2 : 1;
                m_homeText->setText(tr("No file marks"));
            }
            else {
                m_addButton->show();
                currentRow = m_itemMap.size() == 0 ?  2 : 0;
                m_homeText->setText(tr("Please add file to list ~"));
            }

            m_listStack->setCurrentIndex(currentRow);
            m_line->move(m_lineDock->width() / 4 - m_line->width() / 2 + line_pos * (m_lineDock->width() / 2), 0);
        });

        m_line->setFixedSize(24, 4);
        m_line->setStyleSheet(
                    QString("border-top-left-radius:3px;"
                            "border-top-right-radius:3px;"
                            "background-color:%1;")
                            .arg(m_highlightColor.name()));
        m_line->move(m_lineDock->width() / 4 - m_line->width() / 2, 0);

        // 设置悬停提示
        m_addButton->setToolTip(tr("Add file"));
        m_deleteButton->setToolTip(tr("Clear list"));

        setAttribute(Qt::WA_TranslucentBackground);
    }
    resetSize();
}

void PlayListWidget::initMenu()
{
    m_itemMenu = new PlayListItemMenu(nullptr);

    connect(m_itemMenu, &PlayListItemMenu::sigClearList, [this](){
        clearList();
    });

    connect(m_itemMenu, &PlayListItemMenu::sigRemoveSelect, [this](){
        m_playList->deleteFile(m_selectedFile);
    });

    connect(m_itemMenu, &PlayListItemMenu::sigRemoveInvalid, [this](){
        // 删除不可用文件
        m_playList->deleteInvalidItems();
    });

    connect(m_itemMenu, &PlayListItemMenu::sigOpenFolder, [this](){
        QThread::create([this]{
            QProcess p;
            p.start("peony -i \"" + m_selectedFile + "\"");
            p.waitForFinished();
        })->start();

#if 0
        QFileInfo fi(m_selectedFile);

        QString path = fi.absolutePath();
        if(!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
        {
            // 文件夹打开失败
            log_e("openfile %s error!", path.toStdString().c_str());
        }
#endif
    });
}

void PlayListWidget::initPlayList()
{
    m_playList = new PlayList;

    connect(m_playList, &PlayList::itemAdded,       this, &PlayListWidget::addItem);
    connect(m_playList, &PlayList::sigIndexChange,  this, &PlayListWidget::setPlayIndex);
    connect(m_playList, &PlayList::itemDelete,      this, &PlayListWidget::deleteItem);
    connect(m_playList, &PlayList::sigReloadView,   this, &PlayListWidget::reloadView);
    connect(m_playList, &PlayList::sigResetDuration,this, &PlayListWidget::resetDuration);
    connect(m_playList, &PlayList::sigMarkUpdate,   this, &PlayListWidget::updateMarks);
    connect(m_playList, &PlayList::sigMarkClear,    this, &PlayListWidget::clearMark);

    m_playList->initData();
    m_listStack->setCurrentIndex(2);
}

void PlayListWidget::initGlobalSig()
{
    connect(g_user_signal, &GlobalUserSignal::sigShowPlayList, [&](){
        if(m_isShow)
            slotHide();
        else
            slotShow();
    });

    connect(g_user_signal, &GlobalUserSignal::sigHideBar, [&](bool isHide){
        if(isHide)
            if(m_isMouseEnter)
                setCursor(QCursor(Qt::ArrowCursor));
    });

    connect(g_user_signal, &GlobalUserSignal::sigPlayOrder, [&](PlayOrder order){
        m_playOrder = order;
        updateOrderIcon();
    });

#if 0
    connect(g_user_signal, &GlobalUserSignal::sigTheme, [&](int theme){
        switch (theme) {
        case 0:
            if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                setDefaultTheme();
            else
                setBlackTheme();
            break;
        case 1:
            setDefaultTheme();
            break;
        case 2:
            setBlackTheme();
            break;
        default:
            break;
        }

        updateOrderIcon();
        updateViewIcon();
        updateAddIcon();
        updateDeleteIcon();
    });
#endif

    connect(g_core_signal, &GlobalCoreSignal::sigFileNotExist, this, &PlayListWidget::delAvailableIndex, Qt::QueuedConnection);
    connect(g_core_signal, &GlobalCoreSignal::sigFileInfoChange, this, [this](Mpv::FileInfo fi){
        m_currentPlayingFile = fi.file_path;
    });
    connect(g_core_signal, &GlobalCoreSignal::sigFileLoadedError, this, [this](QString file){
        m_playList->setNotAvaliable(file);
        if (m_isShow)
            TipWidget::showTip(tr("Load file error!"), 1000, QSize(150, 40), m_playListWidget, TipWidget::WARNING);
        else
            TipWidget::showTip(tr("Load file error!"), 1000, QSize(150, 40), parentWidget(), TipWidget::WARNING);
    });
    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, this, [this](){
        if (g_playstate == Mpv::Stopped) {
            selectNone();
        }
    });
}

void PlayListWidget::initAnimation()
{
    m_showAnm = new QPropertyAnimation(this, "geometry");
    m_showAnm->setDuration(300);
    m_showAnm->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_showAnm, &QPropertyAnimation::finished, this, &PlayListWidget::updateShowIcon);

    m_hideAnm = new QPropertyAnimation(this, "geometry");
    m_hideAnm->setDuration(300);
    m_hideAnm->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_hideAnm, &QPropertyAnimation::finished, this, &PlayListWidget::updateHideIcon);
}

void PlayListWidget::initConnection()
{
    connect(m_addButton, &QPushButton::clicked, [](){
        g_user_signal->selectFile();
    });

    connect(m_viewButton, &QPushButton::clicked, this, &PlayListWidget::changeListViewMode);

    connect(g_gsettings, &QGSettings::changed, this, &PlayListWidget::gSettingChanged);

    connect(m_leftButton, &QPushButton::pressed, [&](){g_user_signal->showPlayList();});

    connect(m_orderButton, &QPushButton::clicked, [&](){
        // 点击之后直接切换播放模式
        switch (m_playOrder) {
        case ONE_LOOP:
            m_playOrder = LIST_LOOP;
            break;
        case LIST_LOOP:
            m_playOrder = RANDOM;
            break;
        case RANDOM:
            m_playOrder = ONE_LOOP;
            break;
        default:
            break;
        }
        g_user_signal->setPlayOrder(m_playOrder);
    });

    connect(m_deleteButton, &QPushButton::clicked, [&](){
        clearList();
    });
}

void PlayListWidget::initDefaultWidget()
{
    // 默认占位符
    QWidget *defaultWidget = new QWidget;
    QVBoxLayout *ly = new QVBoxLayout(defaultWidget);
    ly->addStretch(2);

    QHBoxLayout *hb = new QHBoxLayout;
    m_homePic = new QWidget;
    m_homePic->setFixedSize(120, 120);
    m_homePic->setStyleSheet("border-image:url(:/ico/list-home-new.png);");

    hb->addStretch();
    hb->addWidget(m_homePic);
    hb->addStretch();
    ly->addLayout(hb);

    m_homeText = new QLabel;
    m_homeText->setStyleSheet("color:#8C8C8C;");
    ly->addWidget(m_homeText);
    m_homeText->setAlignment(Qt::AlignCenter);
    m_homeText->setText(tr("Please add file to list ~"));

    ly->addStretch(3);

//    defaultWidget->show();
    m_listStack->addWidget(defaultWidget);
}

void PlayListWidget::updateOrderIcon()
{
    m_orderButton->setProperty("isWindowButton", 0x1);
    m_orderButton->setProperty("useIconHighlightEffect", 0x2);
    m_orderButton->setFlat(true);
    switch(m_playOrder)
    {
    case ONE_LOOP:
        m_orderButton->setIcon(QIcon::fromTheme("media-playlist-repeat-one-symbolic"));
        m_orderButton->setToolTip(tr("One loop"));
        break;
    case SEQUENCE:
        m_orderButton->setIcon(QIcon::fromTheme("media-playlist-order-symbolic"));
        m_orderButton->setToolTip(tr("Sequential"));
        break;
    case LIST_LOOP:
        m_orderButton->setIcon(QIcon::fromTheme("media-playlist-repeat-symbolic"));
        m_orderButton->setToolTip(tr("List loop"));
        break;
    case RANDOM:
        m_orderButton->setIcon(QIcon::fromTheme("media-playlist-shuffle-symbolic"));
        m_orderButton->setToolTip(tr("Random"));
        break;
    }
}

void PlayListWidget::updateViewIcon()
{
    m_viewButton->setProperty("isWindowButton", 0x1);
    m_viewButton->setProperty("useIconHighlightEffect", 0x2);
    m_viewButton->setFlat(true);
    if(m_mode == LIST) {
        m_viewButton->setIcon(QIcon::fromTheme("view-list-symbolic"));
        m_viewButton->setToolTip(tr("List view"));
    }
    else {
        m_viewButton->setIcon(QIcon::fromTheme("view-grid-symbolic"));
        m_viewButton->setToolTip(tr("Preview view"));
    }
}

void PlayListWidget::updateAddIcon()
{
    m_addButton->setProperty("isWindowButton", 0x1);
    m_addButton->setProperty("useIconHighlightEffect", 0x2);
    m_addButton->setFlat(true);
    if(m_theme == STYLE_LINGMO_LIGHT)
        m_addButton->setIcon(QIcon::fromTheme("list-add-symbolic"));
//        m_addButton->setStyleSheet("QPushButton:!hover{border-image: url(:/ico_light/list-add-symbolic-b.png);}"
//                                    "QPushButton:hover{border-image: url(:/ico_light/list-add-symbolic-h.png);}"
//                                    "QPushButton:pressed{border-image: url(:/ico_light/list-add-symbolic-h.png);}");
    else
        m_addButton->setIcon(QIcon::fromTheme("list-add-symbolic"));
//        m_addButton->setStyleSheet("QPushButton:!hover{border-image: url(:/ico/list-add-symbolic-b.png);}"
//                                    "QPushButton:hover{border-image: url(:/ico/list-add-symbolic-h.png);}"
//                                   "QPushButton:pressed{border-image: url(:/ico/list-add-symbolic-h.png);}");
}

void PlayListWidget::updateDeleteIcon()
{
    m_deleteButton->setProperty("isWindowButton", 0x1);
    m_deleteButton->setProperty("useIconHighlightEffect", 0x2);
    m_deleteButton->setFlat(true);
    if(m_theme == STYLE_LINGMO_LIGHT)
        m_deleteButton->setIcon(QIcon::fromTheme("edit-delete-symbolic"));

//        m_deleteButton->setStyleSheet("QPushButton:!hover{border-image: url(:/ico_light/icon-delect-d.png);}"
//                                     "QPushButton:hover{border-image: url(:/ico_light/icon-delect-h.png);}"
//                                     "QPushButton:pressed{border-image: url(:/ico_light/icon-delect-h.png);}");
    else
        m_deleteButton->setIcon(QIcon::fromTheme("edit-delete-symbolic"));
//        m_deleteButton->setStyleSheet("QPushButton:!hover{border-image: url(:/ico/icon-delect-d.png);}"
//                                     "QPushButton:hover{border-image: url(:/ico/icon-delect-h.png);}"
    //                                      "QPushButton:pressed{border-image: url(:/ico/icon-delect-h.png);}");
}

void PlayListWidget::resetSize()
{
    if (Global::isTablet) {
        m_rightTop->setFixedHeight(60);
        setFixedWidth(WIDGET_WIDTH_TABLET+10);
        m_orderButton->setFixedSize(THEME_BUTTON_SIZE_TABLET, THEME_BUTTON_SIZE_TABLET);
        m_viewButton->setFixedSize(THEME_BUTTON_SIZE_TABLET, THEME_BUTTON_SIZE_TABLET);
        m_addButton->setFixedSize(THEME_BUTTON_SIZE_TABLET, THEME_BUTTON_SIZE_TABLET);
        m_deleteButton->setFixedSize(THEME_BUTTON_SIZE_TABLET, THEME_BUTTON_SIZE_TABLET);
    }
    else {
        m_rightTop->setFixedHeight(48);
        setFixedWidth(WIDGET_WIDTH+10);
        m_orderButton->setFixedSize(THEME_BUTTON_SIZE, THEME_BUTTON_SIZE);
        m_viewButton->setFixedSize(THEME_BUTTON_SIZE, THEME_BUTTON_SIZE);
        m_addButton->setFixedSize(THEME_BUTTON_SIZE, THEME_BUTTON_SIZE);
        m_deleteButton->setFixedSize(THEME_BUTTON_SIZE, THEME_BUTTON_SIZE);
    }
}

void PlayListWidget::updateHightLightColor()
{
    QString title_hover_color = m_isBlackTheme ? "255,255,255" : "0,0,0";

    m_titleList->setStyleSheet(
                QString("QListWidget{background-color: rgba(1,1,1,0);border-radius:10px;}"
                        "QListWidget::item{border-radius:6px;color:#A6A6A6;}"
                        "QListWidget::item::selected{background-color:rgba(1,1,1,0);color:%1;}"
                        "QListWidget::item::hover:!selected{background-color:rgba(%2,20);}")
                        .arg(m_highlightColor.name())
                        .arg(title_hover_color));

    m_line->setStyleSheet(
                QString("border-top-left-radius:3px;"
                        "border-top-right-radius:3px;"
                        "background-color:%1;")
                        .arg(m_highlightColor.name()));

    QString themeColor = g_gsettings->get("themeColor").toString();
    // 侧边箭头样式
    if (m_isShow) {
        m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left2-d.png);}"
                                    "QPushButton:hover{border-image: url(:/%1/icon-left2-%2-h.png);}"
                                    "QPushButton:pressed{border-image: url(:/%1/icon-left2-%2-h.png);}")
                                    .arg(m_icoDir).arg(themeColor));
    }
    else {
        m_leftButton->setStyleSheet(QString("QPushButton:!hover{border-image: url(:/%1/icon-left1-d.png);}"
                                    "QPushButton:hover{border-image: url(:/%1/icon-left1-%2-h.png);}"
                                    "QPushButton:pressed{border-image: url(:/%1/icon-left1-%2-h.png);}")
                                    .arg(m_icoDir).arg(themeColor));
    }
}

bool PlayListWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(m_itemMap.find(watched->objectName()) != m_itemMap.end() && m_isShow)
    {
#if 0
        if(event->type() == QEvent::MouseButtonDblClick)
        {
            // 双击之后播放该条目 需要判断文件是否存在
            if(!((PlayListItem*)watched)->getExist())
            {
                // 文件不存在就不要播放了 弹窗告诉文件不存在
                if (isWayland) {
                    MessageBox box(tr("Video Player"), tr("File not exist!"), nullptr);
                    box.exec();
                }
                else {
                    TipWidget::showTip(tr("File not exist!"), 1000, QSize(150, 40), m_playListWidget, TipWidget::WARNING);
                }
                return false;
            }
            // 再次判断文件是否存在
            QFileInfo fi(watched->objectName());
            if (!fi.exists()) {
                if (isWayland) {
                    MessageBox box(tr("Video Player"), tr("File not exist!"), nullptr);
                    box.exec();
                }
                else {
                    TipWidget::showTip(tr("File not exist!"), 1000, QSize(150, 40), m_playListWidget, TipWidget::WARNING);
                }
                // 借用一下 core 的信号
                m_playList->setNotAvaliable(watched->objectName());
                return false;
            }
            // 双击播放要先停止不然已知播放下一首   双击处理处理  ））））））））））））））））））
            g_user_signal->stop();
            g_user_signal->play();
            g_user_signal->open(watched->objectName());
        }
#endif
        // 进入显示删除按钮，离开隐藏删除按钮
        PlayListItem* t_item = (PlayListItem*)m_playListWidget->itemWidget(m_itemMap[watched->objectName()]);
        if(event->type() == QEvent::Enter)
        {
            if(t_item != nullptr)
                t_item->setEnter();
        }
        else if(event->type() == QEvent::Leave)
        {
            if(t_item != nullptr)
                t_item->setLeave();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void PlayListWidget::enterEvent(QEvent *e)
{
    m_isMouseEnter = true;
    setCursor(Qt::ArrowCursor);
}

void PlayListWidget::leaveEvent(QEvent *e)
{
    m_isMouseEnter = false;
}

void PlayListWidget::moveEvent(QMoveEvent *e)
{
    emit sigMove(parentWidget()->width() - 16 - x());
    return ThemeWidget::moveEvent(e);
}
