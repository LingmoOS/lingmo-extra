#include "textlabel.h"

TextLabel::TextLabel(QWidget *parent) :
    QLabel(parent)
{
}

void TextLabel::updateTextShow()
{
    QFontMetrics fontM(font());
    int rows = (height()+2) / fontM.height();

    QString show_text = "";
    int index = 0;
    while (true) {
        QString tmp_text = "";
        if (rows == 1 || index == m_text.length()) {
            tmp_text = m_text.right(m_text.length() - index);
            show_text += fontM.elidedText(tmp_text, Qt::ElideRight, width());
            break;
        }
        while (true) {
            tmp_text.append(m_text.at(index));
            index++;
            if (fontM.width(tmp_text) > width()) {
                tmp_text.chop(1);
                index--;
                break;
            }
            if (index == m_text.length())
                break;
        }
        show_text += tmp_text;
        if (show_text != m_text)
            show_text.append("\n");
        rows--;
    }
    QLabel::setText(show_text);
}

void TextLabel::resizeEvent(QResizeEvent *e)
{
    QLabel::resizeEvent(e);
    updateTextShow();
}
