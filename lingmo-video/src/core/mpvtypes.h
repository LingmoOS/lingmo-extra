#ifndef MPVTYPES_H
#define MPVTYPES_H

#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>

namespace Mpv
{
    enum Channel{Default, Stereo, Left, Right};
    // filetypes supported by mpv: https://github.com/mpv-player/mpv/blob/master/player/external_files.c
                      // ,"*.m4a","*.aac","*.dts","*.m4r","*.wav","*.wma","*.flv","*.gif"
    const QStringList audio_filetypes = {"*.mp3","*.m4a","*.aac","*.dts","*.m4r","*.wav","*.wma","*.ogg","*.oga","*.ac3","*.ape","*.thd","*.flac","*.ra","*.mka","*.opus","*.mmf","*.mp2","*.wv","*.amr","*.aiff"},
                      video_filetypes = {"*.flv","*.swf","*.h264","*.h265","*.264","*.265","*.avi","*.divx","*.mpg","*.mpeg","*.m1v","*.m2v","*.mpv","*.dv","*.3gp","*.mov","*.mp4","*.m4v","*.mqv","*.dat","*.vcd","*.ogm","*.ogv","*.asf","*.wmv","*.vob","*.mkv","*.ram","*.rm","*.ts","*.rmvb","*.dvr-ms","*.m2t","*.m2ts","*.rec","*.f4v","*.hdmov","*.webm","*.vp8","*.letv","*.hlv","*.mts"},
                      media_filetypes = audio_filetypes + video_filetypes,
                      subtitle_filetypes = {"*.sub","*.srt","*.ass","*.ssa","*.smi","*.rt","*.txt","*.mks","*.vtt","*.sup"};

    enum PlayState
    {
        Idle = -1,
        Started = 1,
        Loaded = 2,
        Playing = 3,
        Paused = 4,
        Stopped = -2
    };

    enum VideoAspect
    {
        AUTO,
        DIV_4_3,
        DIV_16_9,
        FULL
    };

    struct Chapter
    {
        QString title;
        int time;
    };

    struct Track
    {
        int64_t id;
        QString type;
        int64_t src_id;
        QString title;
        QString lang;
        unsigned albumart : 1,
                 _default : 1,
                 external : 1;
        QString external_filename;
        QString codec;

        bool operator==(const Track &t)
        {
            return (id == t.id);
        }
    };

    struct VideoParams
    {
        QString codec;
        int64_t width = 0,
                height = 0,
                dwidth = 0,
                dheight = 0;
        double aspect = 0,
               fps = 0;
    };

    struct AudioParams
    {
        QString codec;
        int samplerate,
            channels;
    };

    struct FileInfo
    {
        QString file_path;
        QString media_title;
        QString file_size;
        QString file_type;
        int length = 0;
        QMap<QString, QString> metadata;
        VideoParams video_params;
        AudioParams audio_params;
        QList<Track> atracks; // audio
        QList<Track> vtracks; // video
        QList<Track> stracks; // subs
        QList<Chapter> chapters;
    };
}
Q_DECLARE_METATYPE(Mpv::PlayState) // so we can pass it with signals & slots
Q_DECLARE_METATYPE(Mpv::Chapter)
Q_DECLARE_METATYPE(Mpv::Track)
Q_DECLARE_METATYPE(Mpv::VideoParams)
Q_DECLARE_METATYPE(Mpv::AudioParams)
Q_DECLARE_METATYPE(Mpv::FileInfo)


#endif // MPVTYPES_H
