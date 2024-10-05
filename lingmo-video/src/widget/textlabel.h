#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include <QLabel>

class TextLabel : public QLabel
{
public:
    TextLabel(QWidget *parent = nullptr);

    void setText(const QString &text) {
        m_text = text;
    }

    void updateTextShow();

private:
    QString m_text;

    void resizeEvent(QResizeEvent *e);
};

#endif // TEXTLABEL_H
