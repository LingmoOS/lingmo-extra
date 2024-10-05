#ifndef SETUPVOLUME_H
#define SETUPVOLUME_H

#include "../themewidget.h"

class CheckBox;

class QLabel;
class QComboBox;
class QHBoxLayout;

class SetupVolume : public ThemeWidget
{
    Q_OBJECT

public:
    explicit SetupVolume(QWidget *parent = nullptr);
    ~SetupVolume();

    void initData();
    void setBlackTheme();
    void setDefaultTheme();
    void setWidgetFont(QString family, int size);

private:
    QLabel *m_title1Label;

    QHBoxLayout *m_hboxLayout;
    QComboBox *m_audioOutputDriver;

    QLabel *m_title2Label;
    CheckBox *m_globalVolumeCheckBox;
    CheckBox *m_standerdVolumeCheckBox;

    void initLayout();
    void initConnect();
};

#endif // SETUPVOLUME_H
