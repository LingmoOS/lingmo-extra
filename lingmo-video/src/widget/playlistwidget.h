#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QStackedWidget>
#include <QWidget>
#include <QMap>

#include <mutex>

#include "themewidget.h"
#include "core/playlist.h"
#include "global/globalsignal.h"

#define FONT_COLOR_SELECTED "rgb(55,144,250)"
#define AnmationDuration 300
#define WIDGET_WIDTH 320
#define WIDGET_WIDTH_TABLET 380

class TextLabel;
class PlayItem;
class PlayList;
class PlayListItemMenu;
class ImageLabel;
class ListLoopMenu;

class QLabel;
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QListWidgetItem;
class QVariantAnimation;
class QPropertyAnimation;

/// 列表模式-预览模式
enum ListViewMode{LIST, VIEW};

class MarkListItem : public ThemeWidget
{
    Q_OBJECT
public:
    explicit MarkListItem(QWidget *parent = nullptr);
    ~MarkListItem();

    int getPos(){return m_markPos;}
    void setData(QString path, int duration, int pos, QString desc, QString preview = "");
    void setMode(ListViewMode mode);
    void setEnter();
    void setLeave();
    QString getFilePath() {return m_tooltip;}
    void setWidgetFont(QFont f);

public slots:
    void setNotExit(QString);
    void setFontColor(QColor);
    void setBlackTheme();
    void setDefaultTheme();

signals:
    void sigPlayAndSeek(QString, int);
    void sigRightBtnPressed(int);
    void sigDeleteMark(QString, int);

private:
    void initStyle();
    void updateNameShow();

private:
    int m_markPos;
    bool m_needSeekToPos,
         m_fileExit;
    QString m_currentPlayingFile;
    QString m_fileName;
    QString m_tooltip;
    QString m_describe;
    QSize m_noPreviewSize, m_previewSize;
    QPixmap *m_pixmapPreview;
    ImageLabel  *m_labPreview;
    QLabel  *m_labFilePath,
            *m_labDuration,
            *m_labMarkPos;
    QWidget *m_dp;

    QVBoxLayout *m_vlay;
    QHBoxLayout *m_hlay;

    QPushButton *m_btnDelete;

    ListViewMode m_mode;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    bool event(QEvent *e) override;
};


/// 播放列表条目
class PlayListItem : public ThemeWidget
{
    Q_OBJECT
public:
    explicit PlayListItem(QWidget *parent = nullptr);
    ~PlayListItem();

    QString getPath() {return m_tip;}
    QString getName() {return m_showName;}
    void setSelected(bool s);
    void setData(QString file, int duration, int lastTime, QString viewPath = "");
    void setDuration(int duration);
    void setEnter();
    void setLeave();
    void toBig();
    void toSmall();
    void reloadView();
    void setMode(ListViewMode mode);
    bool getExist(){return m_isExist;}

    void setBlackTheme();
    void setDefaultTheme();
    void updateHightLightColor();
    void setWidgetFont(const QFont &f);

signals:
    void sigValChange(int);
    void sigRightBPressed(QString);
    void sigNotExists(QString);
    void sigExistsStateChange(QString, bool);

public slots:
    void slotNotExit(QString);

private:
    QLabel  *m_labIcon,
            *m_labSpacing,
            *m_labDuration;
    TextLabel   *m_labName;
    QPixmap *m_pixmapPreview;
    ImageLabel  *m_labPreview;
    QPushButton *m_btnDelete;

    QVariantAnimation   *m_anmToBig,
                        *m_anmToSmall;

    QString m_tip,
            m_showName,
            m_viewPath;
    bool    m_isSelected,
            m_isExist,
            m_isMark;
    ListViewMode m_mode;

    void initStyle();
    void initDeleteButton();
    void initAnimation();
    void updateNameShow();
    void updateText();

private slots:
    void setFontColor(QColor);
    void deleteItem();

protected:
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    bool event(QEvent *e) override;
};

class PlayListWidget : public ThemeWidget
{
    Q_OBJECT
public:
    explicit PlayListWidget(QWidget *parent = nullptr);
    ~PlayListWidget();

    bool isShow(){return m_isShow;}
    PlayList *getPlayList(){return m_playList;}

    void setBlackTheme();
    void setDefaultTheme();

public slots:
    void showItemMenu(QString file);
    void addItem(PlayItem item);
    void deleteItem(QString file);
    void clearList();

    void updateMarks(QVector<MarkItem> markvec);
    void deleteMark(QString filepath, int pos);
    void clearMark();

    void reloadView(QString file);
    void resetDuration(QString file, int duration);
    void slotShow();
    void slotHide();

    // 展开按钮更新
    void setShowButton(bool show);
    void updateShowIcon();
    void updateHideIcon();

    void delAvailableIndex(QString file);
    void changeListViewMode();
    void setPlayIndex(int index);
    void selectNone();

    void gSettingChanged(QString key);

signals:
    void sigAddItem(QString, int, int, QString, bool);
    void sigMove(int);

private:
    QString m_icoDir;

    QWidget *m_homePic;
    QString m_theme;
    QString m_currentPlayingFile;
    QString m_selectedFile;
    PlayList *m_playList;
    QListWidget *m_playListWidget,
                *m_markListWidget;

    PlayOrder m_playOrder;          // 播放顺序
    PlayListItemMenu *m_itemMenu;   // 右击列表选项菜单
    QMap<QString, QListWidgetItem*> m_itemMap;  // 列表item map，key:绝对路径
    QMap<QString, QMap<int, QListWidgetItem*>> m_markMap;

    std::mutex m_mux;
    std::mutex m_itemAddMux;
    bool m_isShow;
    bool m_isMouseEnter;
    bool m_canHideLeftButton;
    ListViewMode m_mode;
    QPropertyAnimation *m_showAnm;
    QPropertyAnimation *m_hideAnm;

    QWidget *m_left;
    QPushButton *m_leftButton;

    QWidget *m_right;
    QWidget *m_rightTop;
    QWidget *m_rightTitle;
    QWidget *m_lineDock;
    QWidget *m_line;
    QLabel *m_homeText;
    QListWidget *m_titleList;
    QStackedWidget *m_listStack;
    QPushButton *m_orderButton;
    QPushButton *m_viewButton;
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;

    void initLayout();
    void initMenu();
//    void initStackedWidget();
    void initPlayList();
    void initGlobalSig();
    void initAnimation();
    void initConnection();
    void initDefaultWidget();

    void updateOrderIcon();
    void updateViewIcon();
    void updateAddIcon();
    void updateDeleteIcon();
    void resetSize();

    void updateHightLightColor();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void moveEvent(QMoveEvent *event) override;
};

#endif // PLAYLISTWIDGET_H
