#ifndef MINIMODESHADE_H
#define MINIMODESHADE_H

#include <QWidget>
#include "widget/filletwidget.h"

#define BTN_LIGHT_BACKGROUND "rgba(249,249,249,77)"
#define BTN_BLACK_BACKGROUND "rgba(31,32,34,77)"

class QPushButton;
class MiniModeButton;

class MiniModeShade : public FilletWidget
{
    Q_OBJECT
public:
    explicit MiniModeShade(QWidget *parent = nullptr);

    void setBlackTheme();
    void setLightTheme();

signals:
    void sigShowNormal();
    void sigPlayPause();
    void sigClose();

private:
    void initLayout();
    void initConnect();

private:
    MiniModeButton  *btnClose,
                    *btnNormal,
                    *btnPlayPause;

protected:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
};

#endif // MINIMODESHADE_H
