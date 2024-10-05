#ifndef MINIMODEBUTTON_H
#define MINIMODEBUTTON_H

#include <QPushButton>

class QLabel;

/** **************************
 * mini 模式下的按钮
 *****************************/

class MiniModeButton : public QPushButton
{
    Q_OBJECT
public:
    MiniModeButton(QString iconName, QSize size, QSize iconSize, QWidget *parent = nullptr);
    ~MiniModeButton();

    void resetName(QString iconName);

    void setLightTheme();
    void setBlackTheme();

private:
    QLabel *labIcon;
    QString iconName;
    bool isBlackTheme;

protected:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
};

#endif // MINIMODEBUTTON_H
