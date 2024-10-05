#ifndef SETUPSYSTEMWIDGET_H
#define SETUPSYSTEMWIDGET_H

#include "../themewidget.h"

class CheckBox;

class QLabel;

class SetupSystem : public ThemeWidget
{
    Q_OBJECT

public:
    explicit SetupSystem(QWidget *parent = nullptr);
    ~SetupSystem();

    void initData();
    void setBlackTheme();
    void setDefaultTheme();
    void setWidgetFont(QString family, int size);

private:
    QLabel *m_title1Label;
    CheckBox *m_mini2trayCheckBox;
    CheckBox *m_pauseWhenMiniCheckBox;
    CheckBox *m_keepStateCheckBox;

    void initLayout();
    void initConnect();
};

#endif // SETUPSYSTEMWIDGET_H
