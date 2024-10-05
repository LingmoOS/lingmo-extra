#include "checkbox.h"

CheckBox::CheckBox(QWidget *parent) :
    QCheckBox(parent)
{

}

CheckBox::~CheckBox()
{

}

void CheckBox::setText(const QString &text)
{
    m_tip = text;
    resetText();
}

void CheckBox::setFont(const QFont &font)
{
    QCheckBox::setFont(font);
    resetText();
}

void CheckBox::resetText()
{
    QFontMetrics fontWidth(font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(m_tip, Qt::ElideRight, width() - 30);
    QCheckBox::setText(show_name);
    if (show_name.endsWith("…"))
        setToolTip(m_tip);
    else
        setToolTip("");
}

void CheckBox::resizeEvent(QResizeEvent *e)
{
    resetText();
    QCheckBox::resizeEvent(e);
}
