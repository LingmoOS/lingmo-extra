#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QColor>
//#include <knavigationbar.h>

//using namespace kdk;

class SetupSystem;
class SetupPlay;
class SetupScreenshot;
class SetupSubtitle;
class SetupVolume;
class SetupCodec;
class SetupShortcut;

namespace Ui {
class SetUpDialog;
}

class SetUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetUpDialog(QWidget *parent = nullptr);
    ~SetUpDialog();
    void setIndex(int index);

private:
    Ui::SetUpDialog *ui;

    double m_windowOpacity;
    QColor m_leftColor;
    QColor m_highlightColor;
    bool m_isBlackTheme;

    SetupSystem     *m_systemPage;
    SetupPlay       *m_playPage;
    SetupScreenshot *m_screenshotPage;
    SetupSubtitle   *m_subtitlePage;
    SetupVolume     *m_volumePage;
    SetupCodec      *m_codecPage;
    SetupShortcut   *m_shortcutPage;

//    KNavigationBar *leftBar;

    void initStyle();
    void initListWidget();
    void initConnect();
    void resetFont();

private slots:
    void slotChangeTheme(bool is_black_theme);

private:
    bool event(QEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void paintEvent(QPaintEvent *e);
    void updateHightLightColor();
};

#endif // SETUPDIALOG_H
