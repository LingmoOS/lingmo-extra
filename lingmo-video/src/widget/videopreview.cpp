#include "videopreview.h"

#include <QResizeEvent>
#include <QtAVWidgets/global.h>

#include "global/global.h"
#include "global/functions.h"

VideoPreview::VideoPreview(QWidget *parent) :
    QDialog(parent)
{
    m_dock = new QWidget(this);

    kdk::LingmoStyleHelper::self()->removeHeader(this);

    m_extractor = new VideoFrameExtractor(this);
    connect(m_extractor, SIGNAL(frameExtracted(QtAV::VideoFrame)), SLOT(displayFrame(QtAV::VideoFrame)));
    connect(m_extractor, SIGNAL(error()), SLOT(displayNoFrame()));
    connect(this, SIGNAL(fileChanged()), SLOT(displayNoFrame()));
    m_extractor->setAutoExtract(false);

    m_out = new VideoOutput(VideoRendererId_Widget, this);
    m_out->widget()->setParent(m_dock);
    m_timeLab = new QLabel(m_dock);
    m_timeLab->setStyleSheet("background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 0), stop:1 rgba(19, 19, 20, 200));color:rgb(222,222,222)");
    m_timeLab->setFixedHeight(25);
    QFont f("Noto Sans CJK SC Regular");
    f.setPixelSize(16);
    m_timeLab->setFont(f);
    m_timeLab->setAlignment(Qt::AlignCenter);

    m_dock->setStyleSheet("border-radius:8px;");
}

void VideoPreview::setFile(QString &file)
{
    if (file == m_file)
        return;
    m_file = file;
    m_extractor->setSource(m_file);
}

void VideoPreview::setTime(qint64 msec)
{
    m_extractor->setPosition(msec);
    QString time_str = Functions::timeToStr((double)msec/1000);
    m_timeLab->setText(time_str);
}

void VideoPreview::display()
{
    m_extractor->extract();
}

void VideoPreview::displayFrame(const VideoFrame &frame)
{
    int diff = qAbs(qint64(frame.timestamp()*1000.0) - m_extractor->position());
    if (diff > m_extractor->precision()) {
        //qWarning("timestamp difference (%d/%lld) is too large! ignore", diff);
    }
    if (m_out->isSupported(frame.format().pixelFormat())) {
        m_out->receive(frame);
        return;
    }
    QSize s = m_out->widget()->rect().size();
    VideoFrame f(frame.to(VideoFormat::Format_RGB32, s));
    if (!f.isValid())
        return;
    m_out->receive(f);
}

void VideoPreview::displayNoFrame()
{
    m_out->receive(VideoFrame());
}

void VideoPreview::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    m_dock->setGeometry(QRect(QPoint(0, 0), e->size()));
    m_out->widget()->setGeometry(QRect(QPoint(0, 0), e->size()));

    m_timeLab->setGeometry(QRect(QPoint(0, height()-25), QSize(width(), 25)));
    m_timeLab->raise();
}
