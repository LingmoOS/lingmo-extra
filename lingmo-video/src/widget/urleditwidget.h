#ifndef URLEDITWIDGET_H
#define URLEDITWIDGET_H

#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class URLEditWidget : public QDialog
{
    Q_OBJECT
public:
    explicit URLEditWidget(QWidget *parent = nullptr);
    ~URLEditWidget();

    QString getUrl(){return m_url;}

private:
    void initLayout();

    QString m_url;

    QVBoxLayout *m_layout_widget;
    QHBoxLayout *m_layout_title;
    QHBoxLayout *m_layout_button;
    QLineEdit *m_line_edit_url;
    QLabel *m_label_title;
    QPushButton *m_button_icon;
    QPushButton *m_button_close;
    QPushButton *m_button_ok;
    QPushButton *m_button_cancel;
};

#endif // URLEDITWIDGET_H
