#ifndef VIDEOPREVIEW_H
#define VIDEOPREVIEW_H

#include <QLabel>
#include <QDialog>
#include <QtAV/VideoOutput.h>
#include <QtAV/VideoFrameExtractor.h>

using namespace QtAV;

class VideoPreview : public QDialog
{
    Q_OBJECT
public:
    explicit VideoPreview(QWidget *parent = nullptr);

    void setFile(QString &file);
    void setTime(qint64 msec);
    void display();
signals:

public slots:
    void displayFrame(const QtAV::VideoFrame& frame);
    void displayNoFrame();

private:
    QString m_file;
    VideoFrameExtractor *m_extractor;
    QWidget *m_dock;

    VideoOutput *m_out;
    QLabel *m_timeLab;
    QLabel *m_imageLab;

    void resizeEvent(QResizeEvent *e) override;
};

#endif // VIDEOPREVIEW_H
