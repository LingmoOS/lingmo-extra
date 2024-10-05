#ifndef SETUPCODEC_H
#define SETUPCODEC_H

#include "../themewidget.h"

class QLabel;
class QLineEdit;
class QComboBox;

class SetupCodec : public ThemeWidget
{
    Q_OBJECT

public:
    explicit SetupCodec(QWidget *parent = nullptr);
    ~SetupCodec();

    void setBlackTheme();
    void setDefaultTheme();
    void setWidgetFont(QString family, int size);

private:
    QLabel *m_label1;
    QComboBox *m_comboBoxOutput;
    QLabel *m_label2;
    QComboBox *m_comboBoxDecoder;
    QLabel *m_label3;
    QLineEdit *m_threadEdit;

    void initLayout();
    void initData();
    void initConnect();
};

#endif // SETUPCODEC_H
