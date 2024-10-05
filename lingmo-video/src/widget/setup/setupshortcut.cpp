#include "setupshortcut.h"

#include <QLabel>
#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

#include "global/global.h"

using namespace Global;

QSet<QString> SetupShortcut::key_set;
QString regKeyString(QString key){
    if(key.endsWith("PgUp"))
        key.replace("PgUp", "Page Up");
    else if(key.endsWith("PgDown"))
        key.replace("PgDown", "Page Down");
    else if(key.endsWith("Return"))
        key.replace("Return", "Enter");
    else if(key.endsWith("空格"))
        key.replace("空格", "Space");
    else if(key.endsWith("{"))
        key.replace("{", "[");
    else if(key.endsWith("}"))
        key.replace("}", "]");
    else if(key.endsWith("~"))
        key.replace("~", "`");
    else if(key.endsWith("!"))
        key.replace("!", "1");
    else if(key.endsWith("@"))
        key.replace("@", "2");
    else if(key.endsWith("#"))
        key.replace("#", "3");
    else if(key.endsWith("$"))
        key.replace("$", "4");
    else if(key.endsWith("%"))
        key.replace("%", "5");
    else if(key.endsWith("^"))
        key.replace("^", "6");
    else if(key.endsWith("&"))
        key.replace("&", "7");
    else if(key.endsWith("*"))
        key.replace("*", "8");
    else if(key.endsWith("("))
        key.replace("(", "9");
    else if(key.endsWith(")"))
        key.replace(")", "0");
    else if(key.endsWith("_"))
        key.replace("_", "-");
    else if(key.endsWith("+")) {
        key.chop(1);
        key.append("=");
    }
    else if(key.endsWith("|"))
        key.replace("|", "\\");
    else if(key.endsWith(":"))
        key.replace(":", ";");
    else if(key.endsWith(":"))
        key.replace(":", ";");
    else if(key.endsWith("\""))
        key.replace("\"", "\'");
    else if(key.endsWith("<"))
        key.replace("<", ",");
    else if(key.endsWith(">"))
        key.replace(">", ".");
    else if(key.endsWith("?"))
        key.replace("?", "/");
    key.replace("+", " + ");
    return key;
}

SetupShortcut::SetupShortcut(QWidget *parent) :
    ThemeWidget(parent)
{
    initLayout();

    initListTitle();
    initFileShortCut();
    initPlayShortCut();
    initImageShortCut();
    initVolumeShortCut();
    initSubtitleShortCut();
    initOtherShortCut();

    m_fileList->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    m_playList->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    m_imageList->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    m_subtitleList->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    m_volumeList->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
    m_otherList->verticalScrollBar()->setProperty("drawScrollBarGroove", false);
}

SetupShortcut::~SetupShortcut()
{
}

void SetupShortcut::setBlackTheme()
{
    m_isBlackTheme = true;

    for(int i=0; i<m_fileList->count(); i++)
        ((ShortCutItem*)m_fileList->itemWidget(m_fileList->item(i)))->setBlackTheme();
    for(int i=0; i<m_playList->count(); i++)
        ((ShortCutItem*)m_playList->itemWidget(m_playList->item(i)))->setBlackTheme();
    for(int i=0; i<m_imageList->count(); i++)
        ((ShortCutItem*)m_imageList->itemWidget(m_imageList->item(i)))->setBlackTheme();
    for(int i=0; i<m_volumeList->count(); i++)
        ((ShortCutItem*)m_volumeList->itemWidget(m_volumeList->item(i)))->setBlackTheme();
    for(int i=0; i<m_subtitleList->count(); i++)
        ((ShortCutItem*)m_subtitleList->itemWidget(m_subtitleList->item(i)))->setBlackTheme();
    for(int i=0; i<m_otherList->count(); i++)
        ((ShortCutItem*)m_otherList->itemWidget(m_otherList->item(i)))->setBlackTheme();

    updateHightLightColor();
}

void SetupShortcut::setDefaultTheme()
{
    m_isBlackTheme = false;

    for(int i=0; i<m_fileList->count(); i++)
        ((ShortCutItem*)m_fileList->itemWidget(m_fileList->item(i)))->setDefaultTheme();
    for(int i=0; i<m_playList->count(); i++)
        ((ShortCutItem*)m_playList->itemWidget(m_playList->item(i)))->setDefaultTheme();
    for(int i=0; i<m_imageList->count(); i++)
        ((ShortCutItem*)m_imageList->itemWidget(m_imageList->item(i)))->setDefaultTheme();
    for(int i=0; i<m_volumeList->count(); i++)
        ((ShortCutItem*)m_volumeList->itemWidget(m_volumeList->item(i)))->setDefaultTheme();
    for(int i=0; i<m_subtitleList->count(); i++)
        ((ShortCutItem*)m_subtitleList->itemWidget(m_subtitleList->item(i)))->setDefaultTheme();
    for(int i=0; i<m_otherList->count(); i++)
        ((ShortCutItem*)m_otherList->itemWidget(m_otherList->item(i)))->setDefaultTheme();

    updateHightLightColor();
}

void SetupShortcut::flushChange()
{
    for(std::pair<QString, QString> p : chg_map)
    {
        QString useless_key = g_shortcut->resetShort(p.first, p.second);
        key_set.insert(regKeyString(p.second));
        key_set.remove(regKeyString(useless_key));
    }
    foreach (auto item, item_set) {
        item->flush();
    }
    chg_map.clear();
}

void SetupShortcut::clearChange()
{
    chg_map.clear();
    foreach (auto item, item_set) {
        item->reset();
    }
}

void SetupShortcut::setWidgetFont(QString family, int size)
{
    QFont f(family);
    f.setPointSize(size);
    foreach (auto item, item_set) {
        item->setWidgetFont(f);
    }

    m_titleList->setFont(f);
}

void SetupShortcut::initLayout()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(30, 10, 24, 8);
    lay->setSpacing(8);

    m_titleList = new QListWidget;
    m_titleList->setFixedHeight(30);
    lay->addWidget(m_titleList);

    {
        m_stackedWidget = new QStackedWidget;
        lay->addWidget(m_stackedWidget);

        m_fileList = new QListWidget;
        m_stackedWidget->addWidget(m_fileList);

        m_playList = new QListWidget;
        m_stackedWidget->addWidget(m_playList);

        m_imageList = new QListWidget;
        m_stackedWidget->addWidget(m_imageList);

        m_volumeList = new QListWidget;
        m_stackedWidget->addWidget(m_volumeList);

        m_subtitleList = new QListWidget;
        m_stackedWidget->addWidget(m_subtitleList);

        m_otherList = new QListWidget;
        m_stackedWidget->addWidget(m_otherList);
    }
}

void SetupShortcut::initListTitle()
{
    m_titleList->setFlow(QListView::LeftToRight);
    m_titleList->addItems(QStringList() <<
                             tr("file") <<
                             tr("play") <<
                             tr("image") <<
                             tr("audio") <<
                             tr("sub") <<
                             tr("other"));

    for(int i=0; i<m_titleList->count(); i++)
    {
        QListWidgetItem *it = m_titleList->item(i);
        it->setTextAlignment(Qt::AlignCenter);
        it->setSizeHint(QSize(67, 30));
    }
    m_titleList->setCurrentRow(0);

    m_fileList->setSpacing(2);
    m_fileList->setStyleSheet(ListWidgetStyle);
    m_fileList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_playList->setSpacing(2);
    m_playList->setStyleSheet(ListWidgetStyle);
    m_playList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_imageList->setSpacing(2);
    m_imageList->setStyleSheet(ListWidgetStyle);
    m_imageList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_volumeList->setSpacing(2);
    m_volumeList->setStyleSheet(ListWidgetStyle);
    m_volumeList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_subtitleList->setSpacing(2);
    m_subtitleList->setStyleSheet(ListWidgetStyle);
    m_subtitleList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_otherList->setSpacing(2);
    m_otherList->setStyleSheet(ListWidgetStyle);
    m_otherList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(14);
    m_titleList->setFont(f);


    initConnect();
}

void SetupShortcut::initConnect()
{
    connect(m_titleList, &QListWidget::currentRowChanged, m_stackedWidget, &QStackedWidget::setCurrentIndex);
}

void SetupShortcut::initFileShortCut()
{
    // key item
    for(std::pair<void(*)(), QShortcut*> pair : g_shortcut->get_short_map())
    {
        // 防止快捷键 new 失败导致崩溃
        if(pair.second == nullptr)
            continue;
        if(pair.second->objectName() == tr("open file") ||
            pair.second->objectName() == tr("open dir") ||
            pair.second->objectName() == tr("prev file") ||
            pair.second->objectName() == tr("next file"))
        {
            QString shortKey = regKeyString(pair.second->key().toString());
            key_set.insert(shortKey);
            QListWidgetItem *it = new QListWidgetItem;
            it->setSizeHint(QSize(400, 35));
            ShortCutItem *itemw = new ShortCutItem(pair.second->objectName(), shortKey, this);
            item_set.insert(itemw);
            connect(itemw, &ShortCutItem::sigKeyChange, this, &SetupShortcut::slotChangeKeyCache);
            m_fileList->addItem(it);
            m_fileList->setItemWidget(it, itemw);
        }
    }
}

void SetupShortcut::initPlayShortCut()
{
    // key item
    for(std::pair<void(*)(), QShortcut*> pair : g_shortcut->get_short_map())
    {
        // 防止快捷键 new 失败导致崩溃
        if(pair.second == nullptr)
            continue;
        if(pair.second->objectName() == tr("play/pause") ||
                pair.second->objectName() == tr("speed up") ||
                pair.second->objectName() == tr("speed down") ||
                pair.second->objectName() == tr("speed normal") ||
                pair.second->objectName() == tr("forword") ||
                pair.second->objectName() == tr("backword") ||
                pair.second->objectName() == tr("forward 30s") ||
                pair.second->objectName() == tr("backword 30s") ||
                pair.second->objectName() == tr("insert bookmark") /*||
                pair.second->objectName() == tr("ib notes")*/)
        {
            QString shortKey = regKeyString(pair.second->key().toString());
            key_set.insert(shortKey);
            QListWidgetItem *it = new QListWidgetItem;
            it->setSizeHint(QSize(400, 35));
            ShortCutItem *itemw = new ShortCutItem(pair.second->objectName(), shortKey, this);
            item_set.insert(itemw);
            connect(itemw, &ShortCutItem::sigKeyChange, this, &SetupShortcut::slotChangeKeyCache);
            m_playList->addItem(it);
            m_playList->setItemWidget(it, itemw);
        }
    }
}

void SetupShortcut::initImageShortCut()
{
    // key item
    for(std::pair<void(*)(), QShortcut*> pair : g_shortcut->get_short_map())
    {
        // 防止快捷键 new 失败导致崩溃
        if(pair.second == nullptr)
            continue;
        if ((g_config->videoOutputType() == GlobalConfig::VO_WID || Global::isTablet) && pair.second->objectName() == tr("mini mode"))
            continue;
        if(pair.second->objectName() == tr("fullscreen") ||
                pair.second->objectName() == tr("mini mode") ||
                pair.second->objectName() == tr("to top") ||
                pair.second->objectName() == tr("screenshot") ||
//                pair.second->objectName() == tr("cut") ||             // 截取先不做实现
                pair.second->objectName() == tr("light up") ||
                pair.second->objectName() == tr("light down") ||
                pair.second->objectName() == tr("forward rotate") ||
                pair.second->objectName() == tr("backward rotate") ||
                pair.second->objectName() == tr("horizontal flip") ||
                pair.second->objectName() == tr("vertical flip") /*||*/
//                pair.second->objectName() == tr("image boost")        // 画质增强先不做实现
                )
        {
            QString shortKey = regKeyString(pair.second->key().toString());
            key_set.insert(shortKey);
            QListWidgetItem *it = new QListWidgetItem;
            it->setSizeHint(QSize(400, 35));
            ShortCutItem *itemw = new ShortCutItem(pair.second->objectName(), shortKey, this);
            item_set.insert(itemw);
            connect(itemw, &ShortCutItem::sigKeyChange, this, &SetupShortcut::slotChangeKeyCache);
            m_imageList->addItem(it);
            m_imageList->setItemWidget(it, itemw);
        }
    }
}

void SetupShortcut::initVolumeShortCut()
{
    // key item
    for (std::pair<void(*)(), QShortcut*> pair : g_shortcut->get_short_map())
    {
        // 防止快捷键 new 失败导致崩溃
        if (pair.second == nullptr)
            continue;
        if (Global::isTablet && pair.second->objectName() == tr("mute"))
            continue;
        if (pair.second->objectName() == tr("volume up") ||
                pair.second->objectName() == tr("volume down") ||
                pair.second->objectName() == tr("mute") ||
                pair.second->objectName() == tr("audio next") ||
                pair.second->objectName() == tr("default channel") ||
                pair.second->objectName() == tr("left channel") ||
                pair.second->objectName() == tr("right channel"))
        {
            QString shortKey = regKeyString(pair.second->key().toString());
            key_set.insert(shortKey);
            QListWidgetItem *it = new QListWidgetItem;
            it->setSizeHint(QSize(400, 35));
            ShortCutItem *itemw = new ShortCutItem(pair.second->objectName(), shortKey, this);
            item_set.insert(itemw);
            connect(itemw, &ShortCutItem::sigKeyChange, this, &SetupShortcut::slotChangeKeyCache);
            m_volumeList->addItem(it);
            m_volumeList->setItemWidget(it, itemw);
        }
    }
}

void SetupShortcut::initSubtitleShortCut()
{
    // key item
    for(std::pair<void(*)(), QShortcut*> pair : g_shortcut->get_short_map())
    {
        // 防止快捷键 new 失败导致崩溃
        if(pair.second == nullptr)
            continue;
        if(pair.second->objectName() == tr("sub load") ||
                pair.second->objectName() == tr("sub earlier") ||
                pair.second->objectName() == tr("sub later") ||
                pair.second->objectName() == tr("sub up") ||
                pair.second->objectName() == tr("sub down") ||
                pair.second->objectName() == tr("sub next"))
        {
            QString shortKey = regKeyString(pair.second->key().toString());
            key_set.insert(shortKey);
            QListWidgetItem *it = new QListWidgetItem;
            it->setSizeHint(QSize(400, 35));
            ShortCutItem *itemw = new ShortCutItem(pair.second->objectName(), shortKey, this);
            item_set.insert(itemw);
            connect(itemw, &ShortCutItem::sigKeyChange, this, &SetupShortcut::slotChangeKeyCache);
            m_subtitleList->addItem(it);
            m_subtitleList->setItemWidget(it, itemw);
        }
    }
}

void SetupShortcut::initOtherShortCut()
{
    // key item
    for(std::pair<void(*)(), QShortcut*> pair : g_shortcut->get_short_map())
    {
        // 防止快捷键 new 失败导致崩溃
        if (pair.second == nullptr)
            continue;
        if (pair.second->objectName() == tr("play list") ||
                pair.second->objectName() == tr("setup"))
        {
            QString shortKey = regKeyString(pair.second->key().toString());
            key_set.insert(shortKey);
            QListWidgetItem *it = new QListWidgetItem;
            it->setSizeHint(QSize(400, 35));
            ShortCutItem *itemw = new ShortCutItem(pair.second->objectName(), shortKey, this);
            item_set.insert(itemw);
            connect(itemw, &ShortCutItem::sigKeyChange, this, &SetupShortcut::slotChangeKeyCache);
            m_otherList->addItem(it);
            m_otherList->setItemWidget(it, itemw);
        }
    }
}

void SetupShortcut::updateHightLightColor()
{
    QString hover_color = m_isBlackTheme ? "#333333" : "#f5f5f5";
    m_titleList->setStyleSheet(
                QString("QListWidget{background-color: rgba(1,1,1,0);border-radius:10px;}"
                        "QListWidget::item{border-radius:6px;color:#8c8c8c;}"
                        "QListWidget::item::selected{background-color:rgba(1,1,1,0);color:%1;}"
                        "QListWidget::item::hover{background-color:%2;}")
                        .arg(m_highlightColor.name())
                        .arg(hover_color));
}

/** **********************************************
 * 快捷键修改临时缓存
 * 说明：只是保存需要修改的哪些快捷键，如果设置窗口点击了
 *      确认按钮，才会真正的修改这些快捷键。
 * @param:name 修改快捷键的名字
 * @param:newKey 要修改的快捷键组合
 * @param:isConflict 是否有冲突 如果有冲突设置为空
 *************************************************/
void SetupShortcut::slotChangeKeyCache(QString name, QString newKey, bool isConflict)
{
    // 临时改变快捷键
    if (!isConflict)
        chg_map[name] = newKey;
}

ShortCutItem::ShortCutItem(QString name, QString key, QWidget *parent) :
    ThemeWidget(parent),
    m_name(name),
    m_key(key),
    m_isConflict(false)
{
    setObjectName(name);
    m_name = name;
    lab_name = new QLabel;
    lab_name->setText(name);
    lab_name->setFixedWidth(130);

    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(16);
    lab_name->setFont(f);

    le_key = new ShortEdit;
    le_key->setReadOnly(true);
    le_key->setText(key);
    le_key->setFixedSize(237, 30);

    /// 设置界面关闭的时候如果没有保存，需要刷新快捷键界面为打开时的快捷键。目前没做。
    connect(le_key, &ShortEdit::sigUpdateShort, [&](QString key){
        emit sigKeyChange(m_name, key, m_isConflict);
    });
    connect(le_key, &ShortEdit::textChanged, [&](QString tmpKey){
        if(tmpKey == m_key)
            setHotkeyConflict(false);
        else
            setHotkeyConflict(SetupShortcut::key_set.find(tmpKey) != SetupShortcut::key_set.end());
    });
    le_key->setCursor(Qt::PointingHandCursor);

    // 热键冲突标签
    lay_key = new QHBoxLayout(le_key);
    lab_conflict = new QLabel;
    lab_conflict->setText(tr("Hotkey conflict"));
    lab_conflict->setStyleSheet("color:#f44e50");
    lay_key->addStretch();
    lay_key->addWidget(lab_conflict);
    lay_key->setContentsMargins(0, 0, 8, 0);
    lab_conflict->hide();

    lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    lay->addWidget(lab_name);
    lay->addWidget(le_key);
    lay->addStretch();

    connectThemeSetting();
}

ShortCutItem::~ShortCutItem()
{
    delete lab_conflict;
    lab_conflict = nullptr;
    delete lay_key;
    lay_key = nullptr;
    delete lab_name;
    lab_name = nullptr;
    delete le_key;
    le_key = nullptr;
    delete lay;
    lay = nullptr;
}

void ShortCutItem::setBlackTheme()
{
    le_key->setBlackTheme();
    lab_name->setStyleSheet("color:#d9d9d9;");
}

void ShortCutItem::setDefaultTheme()
{
    le_key->setDefaultTheme();
    lab_name->setStyleSheet("color:#262626;");
}

void ShortCutItem::setWidgetFont(QFont f)
{
    lab_name->setFont(f);
    lab_conflict->setFont(f);
    le_key->setFont(f);
}

void ShortCutItem::reset()
{
    le_key->setText(m_key);
}

void ShortCutItem::flush()
{
    if(!m_isConflict)
        m_key = le_key->text();
}

void ShortCutItem::setHotkeyConflict(bool isConflict)
{
    m_isConflict = isConflict;
    if(isConflict)
        lab_conflict->show();
    else
        lab_conflict->hide();
}

ShortEdit::~ShortEdit()
{

}

void ShortEdit::setBlackTheme()
{
    m_theme = STYLE_LINGMO_BLACK;
    setStyleSheet("background-color:#404040;color:#d9d9d9;border:0px;border-radius:4px;");
}

void ShortEdit::setDefaultTheme()
{
    m_theme = STYLE_LINGMO_LIGHT;
    setStyleSheet("background-color:#f0f0f0;color:#262626;border:0px;border-radius:4px;");
}

void ShortEdit::setShortCut(QString s)
{
    m_start_text = s;
    setText(m_start_text);
}

ShortEdit::ShortEdit(QWidget *parent) :
    QLineEdit(parent)
{
    setTextMargins(8, 0, 0, 0);
    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(14);
    setFont(f);

    m_highlightColor = QApplication::palette(this).highlight().color();
}

void ShortEdit::updateHightLightColor()
{
    if (hasFocus()) {
        if (m_theme == STYLE_LINGMO_LIGHT) {
            setStyleSheet(QString("QLineEdit{background-color:#f5f5f5;color:#262626;border:1px solid %1;border-radius:4px;}")
                          .arg(m_highlightColor.name()));
        }
        else {
            setStyleSheet(QString("QLineEdit{background-color:#333333;color:#d9d9d9;border:1px solid %1;border-radius:4px;}")
                          .arg(m_highlightColor.name()));
        }
    }
}

void ShortEdit::focusInEvent(QFocusEvent *e)
{
    updateHightLightColor();
    return QLineEdit::focusInEvent(e);
}

void ShortEdit::focusOutEvent(QFocusEvent *e)
{
    if(m_theme == STYLE_LINGMO_LIGHT) {
        setStyleSheet("QLineEdit{background-color:#f0f0f0;color:#262626;border:0px;border-radius:4px;}");
    }
    else {
        setStyleSheet("QLineEdit{background-color:#404040;color:#d9d9d9;border:0px;border-radius:4px;}");
    }

    if(m_start_text != text())
    {
        emit sigUpdateShort(text());
        m_start_text = text();
    }
    return QLineEdit::focusOutEvent(e);
}

void ShortEdit::keyPressEvent(QKeyEvent *e)
{
    // 组合键不能单独去设置
    if (e->key() == Qt::Key_Meta || e->key() == Qt::Key_Shift || e->key() == Qt::Key_Alt || e->key() == Qt::Key_Control)
        return;
    QKeySequence qs(e->key());
    QString keyStr = regKeyString(qs.toString());
    if(e->modifiers() == Qt::ShiftModifier)
    {
        if(e->key() == 16777248)
        {
            setText("Shift");
            return;
        }
        // shift 组合键
        setText("Shift + " + keyStr);
    }
    else if(e->modifiers() == Qt::ControlModifier)
    {
        if(e->key() == 16777249)
        {
            setText("Ctrl");
            return;
        }
        // ctrl 组合键
        setText("Ctrl + " + keyStr);
    }
    else if(e->modifiers() == Qt::AltModifier)
    {
        if(e->key() == 16777251)
        {
            setText("Alt");
            return;
        }
        // ctrl 组合键
        setText("Alt + " + keyStr);
    }
    else if(e->modifiers() == (Qt::AltModifier|Qt::ControlModifier))
    {
        if(e->key() == 16777251 || e->key() == 16777249)
        {
            setText("Ctrl + Alt");
            return;
        }
        // ctrl+alt 组合键
        setText("Ctrl + Alt + " + keyStr);
    }
    else if(e->modifiers() == (Qt::AltModifier|Qt::ShiftModifier))
    {
        if(e->key() == 16777251 || e->key() == 16777248)
        {
            setText("Alt + Shift");
            return;
        }
        // ctrl+shift 组合键
        setText("Alt + Shift + " + keyStr);
    }
    else if(e->modifiers() == (Qt::ControlModifier|Qt::ShiftModifier))
    {
        if(e->key() == 16777249 || e->key() == 16777248)
        {
            setText("Ctrl + Shift");
            return;
        }
        // ctrl+shift 组合键
        setText("Ctrl + Shift + " + keyStr);
    }
    else
    {
        setText(keyStr);
    }
}

bool ShortEdit::event(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange) {
        m_highlightColor = QApplication::palette(this).highlight().color();
    }
    return QLineEdit::event(e);
}
