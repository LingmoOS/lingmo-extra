#include "musicwidget.h"

#include <QTimer>
#include <QLabel>

MusicWidget::MusicWidget(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
    setStyleSheet("QWidget{background-color:#262626;}");

    m_ww = new QWidget(this);
    m_ww->installEventFilter(this);

    m_musicLogo = new QWidget(m_ww);
    m_musicLogo->setFixedSize(MusicLogoSize);
    m_musicLogo->setStyleSheet("border-image:url(:/ico/music-background.png);");

    m_osdFrame = new QLabel(m_ww);
    m_osdFrame->setMargin(30);
    m_osdFrame->installEventFilter(this);
    m_osdFrame->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_osdFrame->setStyleSheet("background-color:rgba(0,0,0,0);color:#ffffff;");
    m_osdFrame->hide();
    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(23);
    m_osdFrame->setFont(f);
    m_osdFrame->raise();

    m_osdShowTimer = new QTimer;
    m_osdShowTimer->setInterval(2000);
    connect(m_osdShowTimer, &QTimer::timeout, [this](){
        m_osdFrame->setText("");
        m_osdFrame->hide();
    });
}

MusicWidget::~MusicWidget()
{

}

void MusicWidget::showOsdText(QString _osd)
{
    m_osdShowTimer->stop();
    m_osdFrame->setText(_osd);
    m_osdFrame->show();
    m_osdFrame->raise();
    m_osdShowTimer->start();
}

void MusicWidget::resizeEvent(QResizeEvent *e)
{
    if (m_osdFrame)
        m_osdFrame->setGeometry(0, 0, width(), height());
    m_ww->setGeometry(0, 0, width(), height());
    if (m_musicLogo) {
        m_musicLogo->move((width() - m_musicLogo->width()) / 2, (height() - m_musicLogo->height()) * 3 / 7);
    }
    return QWidget::resizeEvent(e);
}
