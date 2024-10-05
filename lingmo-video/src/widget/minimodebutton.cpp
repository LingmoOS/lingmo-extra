#include "minimodebutton.h"

#include <QLabel>
#include <QHBoxLayout>

MiniModeButton::MiniModeButton(QString iconName, QSize size, QSize iconSize, QWidget *parent):
    QPushButton(parent),
    iconName(iconName)
{
    labIcon = new QLabel(this);
    setFixedSize(size);
    labIcon->setFixedSize(iconSize);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(labIcon);
}

MiniModeButton::~MiniModeButton()
{

}

void MiniModeButton::resetName(QString iconName)
{
    this->iconName = iconName;
    if(isBlackTheme)
        setBlackTheme();
    else
        setLightTheme();
}

void MiniModeButton::setLightTheme()
{
    isBlackTheme = false;
    if(iconName == "")
        return;
    setStyleSheet(QString("QPushButton{border-radius:%0px;background-color:rgba(249,249,249,120);}").arg(width()/2));
    labIcon->setStyleSheet(QString("border-image:url(:/%0/%1-d.png);").arg("ico_light").arg(iconName));
}

void MiniModeButton::setBlackTheme()
{
    isBlackTheme = true;
    if(iconName == "")
        return;
    setStyleSheet(QString("QPushButton{border-radius:%0px;background-color:rgba(31,32,34,120);}").arg(width()/2));
    labIcon->setStyleSheet(QString("border-image:url(:/%0/%1-d.png);").arg("ico").arg(iconName));
}

void MiniModeButton::enterEvent(QEvent *e)
{
    QString icodir;
    if(isBlackTheme)
    {
        setStyleSheet(QString("QPushButton{border-radius:%0px;background-color:rgba(31,32,34,154);}").arg(width()/2));
        icodir = "ico";
    }
    else
    {
        setStyleSheet(QString("QPushButton{border-radius:%0px;background-color:rgba(249,249,249,154);}").arg(width()/2));
        icodir = "ico_light";
    }
    labIcon->setStyleSheet(QString("border-image:url(:/%0/%1-h.png);").arg(icodir).arg(iconName));
}

void MiniModeButton::leaveEvent(QEvent *e)
{
    QString icodir;
    if(isBlackTheme)
    {
        setStyleSheet(QString("QPushButton{border-radius:%0px;background-color:rgba(31,32,34,120);}").arg(width()/2));
        icodir = "ico";
    }
    else
    {
        setStyleSheet(QString("QPushButton{border-radius:%0px;background-color:rgba(249,249,249,120);}").arg(width()/2));
        icodir = "ico_light";
    }
    labIcon->setStyleSheet(QString("border-image:url(:/%0/%1-d.png);").arg(icodir).arg(iconName));
}
