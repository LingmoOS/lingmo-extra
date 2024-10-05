#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QVBoxLayout>

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = nullptr);
    ~ImageLabel();

    void setTime(QString time);

private:
    QLabel      *m_labTime;
    QVBoxLayout *m_boxLayout;

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // IMAGELABEL_H
