#ifndef SETUPSHORTCUT_H
#define SETUPSHORTCUT_H

#include "setupshortcut.h"
#include "../themewidget.h"

#include <QSet>
#include <QLineEdit>

#define ListWidgetStyle "QListWidget{background-color: rgba(1,1,1,0)}" \
                        "QListWidget::item::selected{background-color:rgba(1,1,1,0);}" \
                        "QListWidget::item::hover{background-color:rgba(1,1,1,0);}"


class QLabel;
class QListWidget;
class QHBoxLayout;
class QStackedWidget;

class ShortEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ShortEdit(QWidget *parent = nullptr);
    ~ShortEdit();

    void setBlackTheme();
    void setDefaultTheme();
    void setShortCut(QString s);
    void setWidgetFont(QString family, int size);

signals:
    void sigUpdateShort(QString);

private:
    QString m_theme;
    QString m_start_text;
    QColor m_highlightColor;

protected:
    void updateHightLightColor();
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    bool event(QEvent *e) override;
};

class ShortCutItem : public ThemeWidget
{
    Q_OBJECT

public:
    explicit ShortCutItem(QString name, QString key, QWidget *parent = nullptr);
    ~ShortCutItem();

    void setBlackTheme();
    void setDefaultTheme();
    void setWidgetFont(QFont f);

    void reset();
    void flush();

public slots:
    void setHotkeyConflict(bool isConflict);

signals:
    /// @param isConflict:如果冲突为 true 目的是为了删除之前未冲突保存的临时修改
    void sigKeyChange(QString name, QString newKey, bool isConflict);
    void sigKeyTextChange(QString tmpKey);

private:
    QString m_name, m_key;
    QHBoxLayout *lay;
    QHBoxLayout *lay_key;
    QLabel *lab_name;
    QLabel *lab_conflict;
    ShortEdit *le_key;
    bool m_isConflict;
};

class SetupShortcut : public ThemeWidget
{
    friend class ShortCutItem;

    Q_OBJECT

public:
    explicit SetupShortcut(QWidget *parent = nullptr);
    ~SetupShortcut();

    void setBlackTheme();
    void setDefaultTheme();
    void updateHightLightColor();
    void flushChange();
    void clearChange();
    void setWidgetFont(QString family, int size);

private:
    QListWidget *m_titleList;
    QStackedWidget *m_stackedWidget;

    QListWidget *m_fileList;
    QListWidget *m_playList;
    QListWidget *m_imageList;
    QListWidget *m_volumeList;
    QListWidget *m_subtitleList;
    QListWidget *m_otherList;

    std::map<QString, QString> chg_map;
    QSet<ShortCutItem*> item_set;
    static QSet<QString> key_set;

    void initLayout();
    void initListTitle();
    void initConnect();

    void initFileShortCut();
    void initPlayShortCut();
    void initImageShortCut();
    void initVolumeShortCut();
    void initSubtitleShortCut();
    void initOtherShortCut();

private slots:
    void slotChangeKeyCache(QString name, QString newKey, bool isConflict);

};

#endif // SETUPSHORTCUT_H
