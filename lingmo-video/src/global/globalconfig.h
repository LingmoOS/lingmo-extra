#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QObject>
#include <QMap>

#include <lingmo-log4qt.h>

// 预设 pcie 设备
struct PciePredefined {
    int vid;
    int pid;
    int cid;
    int score;
    QString description;
};

// pcie 已有设备
struct PcieInfo {
    int vid;
    int pid;
    int cid;
};

class GlobalConfig : public QObject
{
    Q_OBJECT
public:
    enum HardwareDecodecType {
        NO_TYPE = -1,
        DEFAULT_SOFTWARE = 0,
        AMD_SOFTWARE,
        AMD_VAAPI,
        AMD_VDPAU,
        JM7X_VDPAU,
        JM9X_VAAPI,
        GP101_SOFTWARE,
        Sm768_SOFTWARE,
        Nvidia_VDPAU_COPY,
        MooreThreads_VAAPI,
        ZDE_VDPAU_WID,
        ZDE_VDPAU,
        ZDE_VAAPI,
        GPU_709_VAAPI,
        GlenFly_VAAPI,
        X100_GPU,
        INTEL_IrisXe_VAAPI,
        XINDONG_GPU,
        LONGXIN_INTEGRATED_GRAPHICS,
        OTHER_SOFTWARE
    };

    enum VideoOutputType {
        VO_UNKNOW,
        VO_OPENGL,
        VO_WID
    };

    static GlobalConfig* getInstance();
    void reset();

    void clearChange();
    void flushChange();

    HardwareDecodecType hardwareType() {
        return m_hardware_type == NO_TYPE ? getHardwareDecodingType() : m_hardware_type;
    }

    VideoOutputType videoOutputType() {
        if (m_video_output_type == VO_UNKNOW) {
            getHardwareDecodingType();
        }
        KyInfo() << "set video out " << (m_video_output_type == VO_OPENGL ? "opengl" : "wid");
        return m_video_output_type;
    }

    QPair<bool, bool> miniToTray;            // 缩小到托盘
    QPair<bool, bool> pauseWhenMini;         // 最小化时暂停
    QPair<bool, bool> keepStateWhenWakeup;   // 最小化时暂停
    QPair<bool, bool> canRunMultiple;        // 运行多个播放器

    QPair<bool, bool> fullScreenWhenPlay;    // 播放时自动全屏
    QPair<bool, bool> clearListWhenExit;     // 退出时清空列表
    QPair<bool, bool> playLastPos;           // 从上次停止的位置播放
    QPair<bool, bool> playRelationFile;      // 自动查找关联文件播放

    QPair<bool, bool> screenShotSaveToClip;  // 截图保存到剪切板（false：保存到文件）
    QPair<bool, bool> screenShotCurrentSize; // 截图为当前尺寸
    QPair<QString, QString> screenShotPath;  // 截图保存路径
    QPair<QString, QString> screenShotFormat;// 截图格式

    QPair<int, int> subFontSize;             // 字幕字体大小
    QPair<bool, bool> loadSameNameSub;       // 自动载入同名字幕
    QPair<bool, bool> loadAllSubInDir;       // 自动载入其他字幕
    QPair<QString, QString> subDir;          // 字幕载入目录
    QPair<QString, QString> subFontFamily;   // 字幕字体

    QPair<bool, bool> globalVolume;          // 全局音量
    QPair<bool, bool> standardVolume;        // 默认音量标准化
    QPair<QString, QString> audioOut;        // 声音输出选择 如果找不到的话选默认

    QPair<QString, QString> videoDecoder;    // 视频解码器
    QPair<QString, QString> videoOutput;     // 视频输出驱动
    QPair<int, int> videoDecodeThreads;      // 视频解码线程

    QPair<bool, bool> seamlessBrowsing;      // 无痕浏览
    QPair<int, int> audioChannel;            // 声道 0/1/2 | 立体/左/右

signals:

private:
    explicit GlobalConfig(QObject *parent = nullptr);
    static GlobalConfig* instance;

    QList<PciePredefined> predefinedList;
    QList<PcieInfo> pcieList;

private:
    void updatePredefinedList();
    void updatePcieList();
    VideoOutputType m_video_output_type = VO_UNKNOW;
    HardwareDecodecType m_hardware_type = NO_TYPE;
    HardwareDecodecType getHardwareDecodingType();
};

#endif // GLOBALCONFIG_H
