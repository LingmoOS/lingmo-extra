#ifndef FILLETWIDGET_H
#define FILLETWIDGET_H

#include <QWidget>

class FilletWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilletWidget(QWidget *parent = nullptr);
    void setColor(QColor c){m_color = c;}
    void setRadius(int radius){m_radius = radius;repaint();}

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor m_color;
    int m_radius;
};
#endif // FILLETWIDGET_H
