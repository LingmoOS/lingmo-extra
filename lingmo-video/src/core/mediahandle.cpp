#include "mediahandle.h"

#include <ZenLib/Ztring.h>
#include <lingmo-log4qt.h>

using namespace MediaInfoLib;
using namespace ZenLib;

#define wstring2QString(_DATA) \
    QString::fromUtf8(Ztring(_DATA).To_UTF8().c_str())
#define QString2wstring(_DATA) \
    Ztring().From_UTF8(_DATA.toUtf8())

namespace KMedia {

MediaHandle::MediaHandle(const QString &filePath)
{
    m_mediaInfo = new MediaInfo;
    if (filePath != "") {
        m_mediaInfo->Open(QString2wstring(filePath));
        m_filePath = filePath;
        setAll();
    }
}

MediaHandle::~MediaHandle()
{
    unsetAll();
}

void MediaHandle::setFile(const QString &filePath)
{
    unsetAll();
    m_mediaInfo->Close();
    m_filePath = filePath;
    m_mediaInfo->Open(QString2wstring(filePath));
    setAll();
}

quint64 MediaHandle::getVBitRate(int id)
{
    foreach (auto info, m_videoMap) {
        if (info->ID == id)
            return info->Bitrate;
    }
    return -1;
}

quint64 MediaHandle::getABitRate(int id)
{
    foreach (auto info, m_audioMap) {
        if (info->ID == id)
            return info->Bitrate;
    }
    return -1;
}

void MediaHandle::setAll()
{
    if (m_mediaInfo) {
        m_fileSize = wstring2QString(m_mediaInfo->Get(Stream_General, 0, __T("FileSize"))).toULong();
        m_duration = wstring2QString(m_mediaInfo->Get(Stream_General, 0, __T("Duration"))).toULong();
        m_overallBitRate = wstring2QString(m_mediaInfo->Get(Stream_General, 0, __T("OverallBitRate"))).toULong();

        m_videoCount = wstring2QString(m_mediaInfo->Get(Stream_General, 0, __T("VideoCount"))).toInt();
        for (int i = 0; i < m_videoCount; i++) {
            VideoInfo *vinfo = new VideoInfo;
            vinfo->ID        = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("ID"))).toInt();
            vinfo->Width     = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("Width"))).toInt();
            vinfo->Height    = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("Height"))).toInt();
            vinfo->FPSnum    = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("FrameRate_Num"))).toInt();
            vinfo->FPSden    = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("FrameRate_Den"))).toInt();
            vinfo->BitDepth  = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("BitDepth"))).toInt();
            vinfo->Duration  = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("Duration"))).toULong();
            vinfo->CodecID   = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("CodecID")));
            vinfo->Bitrate   = wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("BitRate"))).toULong();
            vinfo->StreamSize= wstring2QString(m_mediaInfo->Get(Stream_Video, i, __T("StreamSize"))).toULong();

            m_duration = m_duration > vinfo->Duration ? m_duration : vinfo->Duration;
            m_videoMap[i] = vinfo;
        }

        m_audioCount = wstring2QString(m_mediaInfo->Get(Stream_General, 0, __T("AudioCount"))).toInt();
        for (int i = 0; i < m_audioCount; i++) {
            AudioInfo *ainfo = new AudioInfo;
            ainfo->ID           = wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("ID"))).toInt();
            ainfo->Channels     = wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("Channels"))).toInt();
            ainfo->Duration     = wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("Duration"))).toULong();
            ainfo->Bitrate      = wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("BitRate"))).toULong();
            ainfo->StreamSize   = wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("StreamSize"))).toULong();
            ainfo->SamplingRate = wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("SamplingRate"))).toULong();
            ainfo->CodecID      = wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("CodecID")));
            ainfo->ChannelLayout= wstring2QString(m_mediaInfo->Get(Stream_Audio, i, __T("ChannelLayout")));

            m_duration = m_duration > ainfo->Duration ? m_duration : ainfo->Duration;
            m_audioMap[i] = ainfo;
        }
    }

    KyInfo() << "File Size:"        << (double)m_fileSize/1024.0/1024.0     << "Mb"
             << "\tDuration:"         << (double)m_duration/1000.0            << "s"
             << "\tm_overallBitRate:"   << (double)m_overallBitRate/1000.0/100.0<< "Mb/s"
             << QString("(%1)").arg(m_filePath);
}

void MediaHandle::unsetAll()
{
    m_filePath = "";
    m_fileSize = 0;
    m_duration = 0;
    m_overallBitRate = 0;

    m_videoCount = 0;
    m_audioCount = 0;
    m_subtitleCount = 0;

    foreach (auto info, m_videoMap) {
        delete info;
        info = nullptr;
    }
    m_videoMap.clear();

    foreach (auto info, m_audioMap) {
        delete info;
        info = nullptr;
    }
    m_audioMap.clear();
}
}
