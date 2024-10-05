#ifndef SETUPPLAYWIDGET_H
#define SETUPPLAYWIDGET_H

#include "../themewidget.h"

class CheckBox;

class SetupPlay : public ThemeWidget
{
    Q_OBJECT

public:
    explicit SetupPlay(QWidget *parent = nullptr);
    ~SetupPlay();

    void initData();
    void setBlackTheme();
    void setDefaultTheme();
    void setWidgetFont(QString family, int size);

private:
    void initConnect();
    void initLayout();

    CheckBox *m_autoFullScreenCheckBox;
    CheckBox *m_clearListExitCheckBox;
    CheckBox *m_findAssociatedPlayCheckBox;
    CheckBox *m_lastPosPlayCheckBox;
};

#endif // SETUPPLAYWIDGET_H
