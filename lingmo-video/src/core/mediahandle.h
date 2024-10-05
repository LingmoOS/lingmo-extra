#ifndef MEDIAHANDLE_H
#define MEDIAHANDLE_H

#include <MediaInfo/MediaInfo.h>

#include <QString>
#include <QMap>

namespace KMedia {

enum VideoFormat {
    H261, H263, H264, H265, RMVB, RMHD, VP8, VP9, MPEG2, MPEG4, AVS, AVC, WMV, MOV
};

enum AudioFormat {
    CDA, WAV, MP3, WMA, RA, MIDI, OGG, APE, FLAC, AAC
};

class VideoInfo {
public:
    VideoInfo(){}
    ~VideoInfo(){}

    int ID;
    int Width;
    int Height;
    int FPSnum;
    int FPSden;
    int BitDepth;

    quint64 Duration;
    quint64 Bitrate;
    quint64 StreamSize;

    QString CodecID;
    VideoFormat Format;
};

class AudioInfo {
public:
    AudioInfo(){}
    ~AudioInfo(){}

    int ID;
    int Channels;

    quint64 Duration;
    quint64 Bitrate;
    quint64 StreamSize;
    quint64 SamplingRate;

    QString CodecID;
    QString ChannelLayout;
    AudioFormat Format;
};

class MediaHandle
{
public:
    MediaHandle(const QString &filePath = "");
    ~MediaHandle();

    void setFile(const QString &filePath);
    int getVideoCount()     {return m_videoCount;}
    int getAudioCount()     {return m_audioCount;}
    int getSubTitleCount()  {return m_subtitleCount;}

    quint64 getDuration()   {return m_duration;}
    quint64 getFileSize()   {return m_fileSize;}
    quint64 getBitRate()    {return m_overallBitRate;}

    quint64 getVBitRate(int id);
    quint64 getABitRate(int id);

private:
    MediaInfoLib::MediaInfo* m_mediaInfo;
    QString m_filePath;
    quint64 m_fileSize;         // kb
    quint64 m_duration;         // ms
    quint64 m_overallBitRate;   // kb/s

    int m_videoCount;
    int m_audioCount;
    int m_subtitleCount;

    QMap<int, VideoInfo*> m_videoMap;
    QMap<int, AudioInfo*> m_audioMap;

    void setAll();
    void unsetAll();
};
}

#endif // MEDIAHANDLE_H
