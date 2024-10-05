#ifndef SETUPSUBTITLE_H
#define SETUPSUBTITLE_H

#include "../themewidget.h"

class CheckBox;

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QHBoxLayout;

class SetupSubtitle : public ThemeWidget
{
    Q_OBJECT

public:
    explicit SetupSubtitle(QWidget *parent = nullptr);
    ~SetupSubtitle();

    void initData();
    void setBlackTheme();
    void setDefaultTheme();
    void setWidgetFont(QString family, int size);

private:
    QLabel *m_label_title_1;
    CheckBox *m_checkbox_load_same;

    QHBoxLayout *m_hboxLayout1;
    QLabel *m_label_path;
    QLineEdit *m_subPathEdit;
    QPushButton *m_browserButton;

    QLabel *m_title2Label;

    QHBoxLayout *m_hboxLayout2;
    QLabel *m_label2;
    QComboBox *m_familyComboBox;

    QHBoxLayout *m_hboxLayout3;
    QLabel *m_label3;
    QComboBox *m_sizeComboBox;

    QString subPath;

    void initLayout();
    void initConnect();

    void paintEvent(QPaintEvent *e);
};

#endif // SETUPSUBTITLE_H
