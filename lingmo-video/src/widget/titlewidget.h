#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>

#define TITLE_BAR_FIXED_HEIGHT 40
#define TITLE_BAR_FIXED_HEIGHT_TBALE 58

class KMenu;
class QLabel;
class QPushButton;

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = nullptr);
    ~TitleWidget();

    int getMenuBtnX();
    void setTitle(QString title);
    void updateMaxButtonStatus(bool is_maxed);
    void setButtonState(bool is_available);

    void setMiniMode(bool mini){m_isMiniMode = mini;}
    void setHide();
    void setShow();
    void setHomePage(bool _isHomePage);

    void setUIMode(bool isTabletMode);

signals:
    void sigMiniMode();
    void sigMiniSize();
    void sigMaxSize();
    void sigShowMenu();
    void sigClose();
    void sigCanHide(bool);
    void sigDBClicked();

private:
    void initLayout();
    void initConnect();
    void resetFont();
    void resetSize();

private:
    QString m_fullTitle;
    QPushButton *btnIcon;
    QLabel  *labTitle;

    QWidget *widget;
    KMenu   *menu;

    QPushButton *m_btnMenu,
                *m_btnMiniMode,
                *m_btnMinSize,
                *m_btnMaxSize,
                *m_btnClose;

    bool m_isMiniMode = false,
         m_isHomePage = true,
         m_leaveState = false;

    bool m_isTabletMode = false;

protected:
    bool event(QEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // TITLEWIDGET_H
