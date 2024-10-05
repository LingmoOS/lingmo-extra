#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#define MusicLogoSize   QSize(360, 120)

#include <QWidget>

class QLabel;

class MusicWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MusicWidget(QWidget *parent = nullptr);
    ~MusicWidget();

public slots:
    void showOsdText(QString _osd);

signals:


private:
    QWidget *m_ww;
    QWidget *m_musicLogo;
    QTimer *m_osdShowTimer;
    QLabel *m_osdFrame;

private:
    void resizeEvent(QResizeEvent *e);
};

#endif // MUSICWIDGET_H
