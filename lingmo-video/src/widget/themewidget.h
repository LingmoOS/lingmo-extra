#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QWidget>
#include <QApplication>

#include "global/global.h"

using namespace Global;

class ThemeWidget : public QWidget
{
    friend class ContralBar;
    friend class SetupShortcut;
    friend class PlayListItem;
    friend class MarkListItem;
    friend class PlayListWidget;

    Q_OBJECT
public:
    explicit ThemeWidget(QWidget *parent = nullptr):
        QWidget(parent)
    {
        m_highlightColor = QApplication::palette(this).highlight().color();
    }
    ~ThemeWidget(){}

    void connectThemeSetting(){
        if (FOLLOW_SYS_THEME) {
            if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                setDefaultTheme();
            else
                setBlackTheme();

            connect(g_gsettings, &QGSettings::changed, this, [&](QString key){
                // 如果不是跟随主题的话直接返回
                if (key == "styleName") {
                    if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                        setDefaultTheme();
                    else
                        setBlackTheme();
                }
            });
        }
        else {
            setBlackTheme();
        }
    }

    virtual void setBlackTheme(){}
    virtual void setDefaultTheme(){}
    virtual void updateHightLightColor(){}

private:
    bool m_isBlackTheme;
    QColor m_highlightColor;
    bool event(QEvent *event) override{
        if (event->type() == QEvent::PaletteChange) {
            m_highlightColor = QApplication::palette(this).highlight().color();
            updateHightLightColor();
        }
        return QWidget::event(event);
    }
};

#endif // THEMEWIDGET_H
