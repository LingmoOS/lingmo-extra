#ifndef TIPWIDGET_H
#define TIPWIDGET_H

#include <QDialog>

class TipWidget : public QDialog
{
    Q_OBJECT
public:
    enum TipType {
        ERROR,
        OK,
        WARNING
    };

    explicit TipWidget(QWidget *parent = nullptr);
    ~TipWidget();

    QWidget *getWidget(){return w;}
    void setLightTheme();
    void setBlackTheme();

    static void showTip(QString tip, int show_time, QSize size, QWidget* parent = nullptr, TipType type = OK);

signals:

private:
    void resizeEvent(QResizeEvent *event) override;

    QWidget *w;
};

#endif // TIPWIDGET_H
