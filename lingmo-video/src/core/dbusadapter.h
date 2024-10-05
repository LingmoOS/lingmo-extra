#ifndef DBUSADAPTER_H
#define DBUSADAPTER_H

#include <QObject>
#include "core/mpvtypes.h"

class DbusAdapter : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
public:
    explicit DbusAdapter(QObject *parent = nullptr);

public slots:
    // mpris 使用
    void Stop() const;
    void Next() const;
    // 这个改个名字，不能用 PlayPause，不然会和线控冲突
    void KvPlayPause() const;

    void PlayPause() const;

    void Previous() const;
    void AddFile(QString file) const;
    void VolumeUp() const;
    void VolumeDown() const;
    void FullScreen() const;
    void LoopMode() const;
    void Exit() const;

    int PlayState();
    QString CurrentFile();

private:
    QString m_currentFile;

signals:

};

#endif // DBUSADAPTER_H
