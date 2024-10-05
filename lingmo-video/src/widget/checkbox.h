#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QCheckBox>

class CheckBox : public QCheckBox
{
    Q_OBJECT
public:
    CheckBox(QWidget *parent = nullptr);
    ~CheckBox();

    void setText(const QString &text);
    void setFont(const QFont &font);

private:
    QString m_tip;

    void resetText();
    void resizeEvent(QResizeEvent *e) override;
};

#endif // CHECKBOX_H
