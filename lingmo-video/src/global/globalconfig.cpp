#include "globalconfig.h"
#include "global/global.h"
#include "global/globalsignal.h"
#include "global/functions.h"

#include <thread>
#include <QStandardPaths>

using namespace Global;

GlobalConfig* GlobalConfig::instance = nullptr;

GlobalConfig *GlobalConfig::getInstance()
{
    if(instance == nullptr)
        instance = new GlobalConfig;
    return instance;
}

void GlobalConfig::clearChange()
{
    miniToTray.second           = miniToTray.first;
    pauseWhenMini.second        = pauseWhenMini.first;
    canRunMultiple.second       = canRunMultiple.first;
    keepStateWhenWakeup.second  = keepStateWhenWakeup.first;

    playLastPos.second          = playLastPos.first;
    playRelationFile.second     = playRelationFile.first;
    clearListWhenExit.second    = clearListWhenExit.first;
    fullScreenWhenPlay.second   = fullScreenWhenPlay.first;

    screenShotPath.second           = screenShotPath.first;
    screenShotFormat.second         = screenShotFormat.first;
    screenShotSaveToClip.second     = screenShotSaveToClip.first;
    screenShotCurrentSize.second    = screenShotCurrentSize.first;

    subDir.second           = subDir.first;
    subFontSize.second      = subFontSize.first;
    subFontFamily.second    = subFontFamily.first;
    loadSameNameSub.second  = loadSameNameSub.first;
    loadAllSubInDir.second  = loadAllSubInDir.first;

    audioOut.second         = audioOut.first;
    globalVolume.second     = globalVolume.first;
    standardVolume.second   = standardVolume.first;

    videoDecoder.second         = videoDecoder.first;
    videoOutput.second          = videoOutput.first;
    videoDecodeThreads.second   = videoDecodeThreads.first;

    seamlessBrowsing.second = seamlessBrowsing.first;
    audioChannel.second = audioChannel.first;
}

void GlobalConfig::flushChange()
{
    bool needRestart = false;
    // 是否最小化到托盘
    if (miniToTray.first != miniToTray.second) {
        miniToTray.first = miniToTray.second;
        g_settings->setValue("General/mini_to_tray", miniToTray.second);
    }

    // 最小化时暂停播放
    if (pauseWhenMini.first != pauseWhenMini.second) {
        pauseWhenMini.first = pauseWhenMini.second;
        g_settings->setValue("General/pause_when_mini", pauseWhenMini.second);
    }

    if (keepStateWhenWakeup.first != keepStateWhenWakeup.second) {
        keepStateWhenWakeup.first = keepStateWhenWakeup.second;
        g_settings->setValue("General/keep_state_when_wakeup", keepStateWhenWakeup.second);
    }

    // 是否可运行多个播放器
    if (canRunMultiple.first != canRunMultiple.second) {
        canRunMultiple.first = canRunMultiple.second;
        g_settings->setValue("General/can_run_multiple", canRunMultiple.second);
    }

    // 播放时自动全屏
    if (fullScreenWhenPlay.first != fullScreenWhenPlay.second) {
        fullScreenWhenPlay.first = fullScreenWhenPlay.second;
        g_settings->setValue("General/fullscreen_when_play", fullScreenWhenPlay.second);
    }

    // 退出时清空播放列表
    if (clearListWhenExit.first != clearListWhenExit.second) {
        clearListWhenExit.first = clearListWhenExit.second;
        g_settings->setValue("General/clearlist_when_exit", clearListWhenExit.second);
    }

    // 从上次停止的地方播放
    if (playLastPos.first != playLastPos.second) {
        playLastPos.first = playLastPos.second;
        g_settings->setValue("General/play_last_pos", playLastPos.second);
    }

    // 自动播放相关文件
    if (playRelationFile.first != playRelationFile.second) {
        playRelationFile.first = playRelationFile.second;
        g_settings->setValue("General/play_relation_file", playRelationFile.second);
    }

    // 截图保存为文件
    if (screenShotSaveToClip.first != screenShotSaveToClip.second) {
       screenShotSaveToClip.first = screenShotSaveToClip.second;
       g_settings->setValue("General/screenshot_save_to_file", screenShotSaveToClip.second);
    }

    // 截图保存路径
    if (screenShotPath.first != screenShotPath.second) {
        screenShotPath.first = screenShotPath.second;
        g_settings->setValue("General/screenshot_path", screenShotPath.second);
        g_user_signal->screenShotDir(screenShotPath.second);
    }

    // 截图格式
    if (screenShotFormat.first != screenShotFormat.second) {
        screenShotFormat.first = screenShotFormat.second;
        g_settings->setValue("General/screenshot_format", screenShotFormat.second);
        g_user_signal->screenShotFormat(screenShotFormat.second);
    }

    // 截图尺寸
    if (screenShotCurrentSize.first != screenShotCurrentSize.second) {
        screenShotCurrentSize.first = screenShotCurrentSize.second;
        g_settings->setValue("General/screenshot_size", screenShotCurrentSize.second);
    }

    // 自动载入同名字幕
    if (loadSameNameSub.first != loadSameNameSub.second) {
        loadSameNameSub.first = loadSameNameSub.second;
        g_settings->setValue("General/load_same_name_subtitle", loadSameNameSub.second);
    }

    // 自动载入文件夹下其他字幕
    if (loadAllSubInDir.first != loadAllSubInDir.second) {
        loadAllSubInDir.first = loadAllSubInDir.second;
        g_settings->setValue("General/load_all_dir_subtitle", loadAllSubInDir.second);
    }

    // 字幕载入路径
    if (subDir.first != subDir.second) {
        subDir.first = subDir.second;
        g_settings->setValue("General/subtitle_dir", subDir.second);
    }

    // 字幕字体
    if (subFontFamily.first != subFontFamily.second) {
        subFontFamily.first = subFontFamily.second;
        g_settings->setValue("General/subtitle_font_family", subFontFamily.second);
        g_user_signal->setSubFont(subFontFamily.first);
    }

    // 字幕字体大小
    if (subFontSize.first != subFontSize.second) {
        subFontSize.first = subFontSize.second;
        g_settings->setValue("General/subtitle_font_size", subFontSize.second);
        g_user_signal->setSubSize(subFontSize.first);
    }

    // 音频输出驱动
    if (audioOut.first != audioOut.second) {
        // 输出驱动无法在播放的时候修改
        audioOut.first = audioOut.second;
        g_settings->setValue("General/audio_output_device", audioOut.second);
        needRestart = true;
    }

    // 全局音量
    if (globalVolume.first != globalVolume.second) {
        globalVolume.first = globalVolume.second;
        g_settings->setValue("General/global_volume", globalVolume.second);
    }

    // 默认音量标准化
    if (standardVolume.first != standardVolume.second) {
        standardVolume.first = standardVolume.second;
        g_settings->setValue("General/standard_volume", standardVolume.second);
    }

    // 视频解码器
    if (videoDecoder.first != videoDecoder.second) {
        videoDecoder.first = videoDecoder.second;
        g_settings->setValue("General/video_decoder", videoDecoder.second);
        needRestart = true;
    }

    // 视频输出驱动
    if (videoOutput.first != videoOutput.second) {
        videoOutput.first = videoOutput.second;
        g_settings->setValue("General/video_output", videoOutput.second);
        needRestart = true;
    }

    if (videoDecodeThreads.first != videoDecodeThreads.second) {
        videoDecodeThreads.first = videoDecodeThreads.second;
        g_settings->setValue("General/video_decode_threads", videoDecodeThreads.second);
        needRestart = true;
    }

    //无痕浏览
    if (seamlessBrowsing.first != seamlessBrowsing.second) {
        seamlessBrowsing.first = seamlessBrowsing.second;
        g_settings->setValue("General/seamless_browsing", seamlessBrowsing.second);
    }

    // 声道
    if (audioChannel.first != audioChannel.second) {
        audioChannel.first = audioChannel.second;
        g_settings->setValue("General/audio_channel", audioChannel.second);
    }

    if (needRestart) {
        g_user_signal->restart();
    }
}

GlobalConfig::GlobalConfig(QObject *parent) : QObject(parent)
{
    reset();
}

void GlobalConfig::updatePredefinedList()
{
    //# 对于PCIe设备的预处理
    //# 格式："VID:PID:ClassID:预设分数:描述信息"
    //# 一行一条规则
    //# VID、PID中，-1或者0xFFFF代表匹配所有
    //# ClassID中，-1或者0xFFFFFF代表匹配所有
    //# 例如："0x8086:0x1901:0x0:0:Intel PCIe Controller (x16)"

    QStringList itemList;
    itemList.append("1002:6611:-1:300:AMD Graphics Card R7");
    itemList.append("1002:6613:-1:300:AMD Graphics Card R7");
    // 下面这个显卡老，共性说不走硬解，不然的话 s3 回来会卡，需要重启才行
    itemList.append("1002:6766:-1:300:AMD Graphics Card Caicos");

    // 这个显卡也不走硬解，共性确认过
    itemList.append("1002:6779:-1:300:AMD Graphics Card Caicos");

    itemList.append("1002:-1:-1:300:AMD Graphics Card");
    itemList.append("0709:0201:30000:0:709 Graphics Card");
    itemList.append("10de:-1:-1:0:Nvidia Graphics Card");
    itemList.append("1ed5:0101:-1:0:Moore Threads Graphics Card");
    itemList.append("1ed5:0102:-1:0:Moore Threads Graphics Card");
    itemList.append("1ed5:0105:-1:0:Moore Threads Graphics Card");
    itemList.append("1ed5:0106:-1:0:Moore Threads Graphics Card");
    itemList.append("8086:9a49:-1:0:Iris Xe Graphics");
    itemList.append("0731:-1:-1:0:JINGJIA MICRO Graphics Card");
//    itemList.append("126f:-1:-1:0:SM750/SM768");  // 暂时不要这个显卡判断
    itemList.append("0709:0001:-1:0:709 GP101 Graphics Card");
    itemList.append("1d17:3d00:30000:0:ZhaoXin Graphics Card");
    itemList.append("1d17:3d01:30000:0:ZhaoXin Graphics Card");
    itemList.append("1d17:3a04:30000:0:ZhaoXin Graphics Card");
    itemList.append("6766:3d00:30000:0:GlenFly Graphics Card");
    itemList.append("6766:3d02:30000:0:GlenFly Graphics Card");
    itemList.append("1db7:dc20:-1:0:X100 Series");
    itemList.append("1ec8:9800:-1:0:XinDong Card");
    itemList.append("0014:7a06:-1:0:LongXin 7a1000 Card");
    itemList.append("0014:7a36:-1:0:LongXin 7a2000 Card");
    itemList.append("1a03:-1:-1:0:BMCd");

    foreach (QString item, itemList) {
        item = item.trimmed();
        if (item.startsWith("#"))
            continue;
        if (item.startsWith("//"))
            continue;

        PciePredefined device;
        QStringList stringList = item.split(":");
        bool ok;

        if (stringList.count() < 5)
            continue;

        device.vid = stringList[0].toInt(&ok, 16);
        if (device.vid == -1)
            device.vid = 0xFFFF;
        stringList.removeAt(0);

        device.pid = stringList[0].toInt(&ok, 16);
        if (device.pid == -1)
            device.pid = 0xFFFF;
        stringList.removeAt(0);

        device.cid = stringList[0].toInt(&ok, 16);
        if (device.cid == -1)
            device.cid = 0xFFFFFF;
        stringList.removeAt(0);

        device.score = stringList[0].toInt(&ok, 10);
        stringList.removeAt(0);

        device.description = stringList.join(":").trimmed();
        predefinedList.append(device);
    }
}

void GlobalConfig::updatePcieList()
{
    QDir dir("/sys/bus/pci/devices/");
    if (!dir.exists())
            return;

    dir.setFilter(QDir::Dirs);
    QStringList busList = dir.entryList();
    busList.removeOne(".");
    busList.removeOne("..");

    foreach(QString bus, busList) {
        PcieInfo info;
        QString path;
        QFile file;
        QByteArray charArray;
        bool ok;
        int id;

        path = dir.absoluteFilePath(bus + "/" + "vendor");
        file.setFileName(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        charArray = file.readAll();
        file.close();
        id = QString(charArray).toInt(&ok, 16);
        info.vid = id;

        path = dir.absoluteFilePath(bus + "/" + "device");
        file.setFileName(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        charArray = file.readAll();
        file.close();
        id = QString(charArray).toInt(&ok, 16);
        info.pid = id;

        path = dir.absoluteFilePath(bus + "/" + "class");
        file.setFileName(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        charArray = file.readAll();
        file.close();
        id = QString(charArray).toInt(&ok, 16);
        info.cid = id;

        pcieList.append(info);
    }
}

GlobalConfig::HardwareDecodecType GlobalConfig::getHardwareDecodingType()
{
    //默认值为-1,表示使用软解
    m_hardware_type = DEFAULT_SOFTWARE;
    m_video_output_type = VO_OPENGL;

    updatePredefinedList();

    int size = predefinedList.size();
    if (size > 0) {
        updatePcieList();

        foreach (PciePredefined predefined, predefinedList) {
            foreach(PcieInfo info, pcieList) {
                if ( ((predefined.vid == info.vid) || (predefined.vid == 0xFFFF))
                         && ((predefined.pid == info.pid) || (predefined.pid == 0xFFFF))
                         && ((predefined.cid == info.cid) || (predefined.cid == 0xFFFFFF))) {
                    KyInfo("Find %s device(%04x:%04x.%04x), "
                               "use predefine score: %d\n",
                               predefined.description.toUtf8().data(),
                               info.vid, info.pid,
                               info.cid, predefined.score);
                    char vidstr[128] = {0}, pidstr[128] = {0}, cidstr[128] = {0};
                    snprintf(vidstr, sizeof(vidstr), "%04x", info.vid);
                    snprintf(pidstr, sizeof(pidstr), "%04x", info.pid);
                    snprintf(cidstr, sizeof(cidstr), "%04x", info.cid);
                    QString vid = QString::fromStdString(std::string(vidstr));
                    QString pid = QString::fromStdString(std::string(pidstr));
                    QString cid = QString::fromStdString(std::string(cidstr));
                    //Find AMD Graphics Card device(1002:ffff.ffffff), use predefine score: 300
                    if (predefined.description == "AMD Graphics Card R7" && vid == "1002" && (pid == "6611" || pid == "6613")) {
                        m_video_output_type = VO_OPENGL;
                        m_hardware_type = AMD_SOFTWARE;
                        KyInfo() << "AMD_SOFTWARE";
                    }
                    if (predefined.description == "AMD Graphics Card Caicos" && vid == "1002" && (pid == "6766" || pid == "6779")) {
                        m_video_output_type = VO_OPENGL;
                        m_hardware_type = AMD_SOFTWARE;
                        KyInfo() << "AMD_SOFTWARE";
                    }
                    if (predefined.description == "AMD Graphics Card Lexa" && vid == "1002" && pid == "6987") {
                        m_video_output_type = VO_WID;
                        m_hardware_type = AMD_VAAPI;
                        KyInfo() << "AMD_VAAPI";
                    }
                    else if (predefined.description == "AMD Graphics Card" && vid == "1002") {
                        m_video_output_type = VO_OPENGL;
                        m_hardware_type = AMD_VDPAU;
                        KyInfo() << "AMD_VDPAU";
                    }
                    else if (predefined.description == "Iris Xe Graphics" && vid == "8086" && pid == "9a49") {
                        m_video_output_type = VO_OPENGL;
                        m_hardware_type = INTEL_IrisXe_VAAPI;
                        KyInfo() << "INTEL_IrisXe_VAAPI";
                    }
                    else if (predefined.description == "Moore Threads Graphics Card"
                             && vid == "1ed5"
                             && (pid == "0101" || pid == "0102" || pid == "0105" || pid == "0106")) {
                        m_video_output_type = VO_WID;
                        m_hardware_type = MooreThreads_VAAPI;
                        KyInfo() << "MooreThreads_VAAPI";
                    }
                    else if (predefined.description == "JINGJIA MICRO Graphics Card" && vid == "0731" && pid.startsWith("9")) {
                        m_video_output_type = VO_WID;
                        m_hardware_type = JM9X_VAAPI;
                        KyInfo() << "JM9X_VAAPI";
                    }
                    else if (predefined.description == "JINGJIA MICRO Graphics Card" && vid == "0731" && pid.startsWith("7")) {
                        m_video_output_type = VO_WID;
                        m_hardware_type = JM7X_VDPAU;
                        KyInfo() << "JM7X_VDPAU";
                    }
                    else if (predefined.description == "709 GP101 Graphics Card" && vid == "0709") {
                        m_video_output_type = VO_OPENGL;
                        m_hardware_type = GP101_SOFTWARE;
                        KyInfo() << "GP101_SOFTWARE";
                    }
                    else if (predefined.description == "ZhaoXin Graphics Card"
                             && vid == "1d17"
                             && pid == "3d01"
                             && cid == "30000") {
                        m_video_output_type = VO_WID;
                        m_hardware_type = ZDE_VAAPI; // zxe3d01, 默认走 VAAPI 硬件解码
                        KyInfo() << "ZDE_VAAPI";
                    }
                    else if (predefined.description == "ZhaoXin Graphics Card"
                             && vid == "1d17"
                             && pid == "3d00"
                             && cid == "30000") {
                        m_video_output_type = VO_OPENGL;
                        m_hardware_type = ZDE_VDPAU; // zxe3d00, 默认走 VDPAU 硬件解码
                        KyInfo() << "ZDE_VDPAU";
                    }
                    else if (predefined.description == "ZhaoXin Graphics Card"
                             && vid == "1d17"
                             && pid == "3a04"
                             && cid == "30000") {
                        m_video_output_type = VO_WID;
                        m_hardware_type = ZDE_VAAPI; // zxe3a04, 默认走 VAAPI 硬件解码
                        KyInfo() << "ZDE_VAAPI";
                    }
                    else if (predefined.description == "GlenFly Graphics Card"
                             && vid == "6766"
                             && (pid == "3d00" || pid == "3d02")
                             && cid == "30000") {
                        // 格兰菲显卡 vaapi 输出效果好一点，很多显卡都是 vaapi 输出效果优于 opengl，mpv可能需要优化，支持vaapi输出
                        m_video_output_type = VO_WID;
                        m_hardware_type = GlenFly_VAAPI; // 格兰菲显卡, 默认走 VAAPI 硬件解码
                        KyInfo() << "GlenFly_VAAPI";
                    }
                    else if (predefined.description == "X100 Series" && vid == "1db7" && pid == "dc20") {
                        m_video_output_type = VO_WID;
                        m_hardware_type = X100_GPU; // X100
                        KyInfo() << "X100_GPU";
                    }
                    else if (predefined.description == "Nvidia Graphics Card" && vid == "10de") {
                        m_video_output_type = VO_OPENGL;
                        m_hardware_type = Nvidia_VDPAU_COPY;
                        KyInfo() << "Nvidia_VDPAU_COPY";
                    }
                    else if (predefined.description == "709 Graphics Card" && vid == "0709" && pid == "0201" && cid == "30000") {
                        m_video_output_type = VO_WID;
                        m_hardware_type = GPU_709_VAAPI;
                        KyInfo() << "GPU_709_VAAPI";
                    }
                    else if (predefined.description == "SM750/SM768" && vid == "126f") {
                        m_video_output_type = VO_WID;
                        m_hardware_type = Sm768_SOFTWARE;
                        KyInfo() << "Sm768_SOFTWARE";
                    }
                    else if (predefined.description == "XinDong Card" && vid == "1ec8") {
                        m_video_output_type = VO_WID;
                        m_hardware_type = XINDONG_GPU;
                        KyInfo() << "XINDONG_GPU";
                    }
                    else if ((predefined.description == "LongXin 7a1000 Card" || predefined.description == "LongXin 7a2000 Card")
                             && vid == "0014"
                             && (pid == "7a06" || pid == "7a36")) {
                        // 龙芯集显不要直接返回接着循环判断是否有独显
                        m_video_output_type = VO_WID;
                        m_hardware_type = LONGXIN_INTEGRATED_GRAPHICS;
                        KyInfo() << "LONGXIN_INTEGRATED_GRAPHICS";
                        continue;
                    }
                    return m_hardware_type;
                }
            }
        }
    }

    return m_hardware_type;
}

void GlobalConfig::reset()
{
    miniToTray.first            = miniToTray.second             = g_settings->value("General/mini_to_tray").toBool();
    pauseWhenMini.first         = pauseWhenMini.second          = g_settings->value("General/pause_when_mini").toBool();
    if (!g_settings->contains("General/keep_state_when_wakeup"))
        keepStateWhenWakeup.second = true;
    else
        keepStateWhenWakeup.first = keepStateWhenWakeup.second  = g_settings->value("General/keep_state_when_wakeup").toBool();
    canRunMultiple.first        = canRunMultiple.second         = g_settings->value("General/can_run_multiple").toBool();

    fullScreenWhenPlay.first    = fullScreenWhenPlay.second = g_settings->value("General/fullscreen_when_play").toBool();
    clearListWhenExit.first     = clearListWhenExit.second  = g_settings->value("General/clearlist_when_exit").toBool();
    playLastPos.first           = playLastPos.second        = g_settings->value("General/play_last_pos").toBool();
    playRelationFile.first      = playRelationFile.second   = g_settings->value("General/play_relation_file").toBool();

    screenShotSaveToClip.first  = screenShotSaveToClip.second   = g_settings->value("General/screenshot_save_to_file").toBool();
    screenShotPath.first        = screenShotPath.second         = g_settings->value("General/screenshot_path").toString();
    if (screenShotPath.first.length() == 0) {
        screenShotPath.second = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
    screenShotFormat.first      = screenShotFormat.second       = g_settings->value("General/screenshot_format").toString();
    screenShotCurrentSize.first = screenShotCurrentSize.second  = g_settings->value("General/screenshot_size").toBool();

    loadSameNameSub.first   = loadSameNameSub.second    = g_settings->value("General/load_same_name_subtitle").toBool();
    loadAllSubInDir.first   = loadAllSubInDir.second    = g_settings->value("General/load_all_dir_subtitle").toBool();
    subDir.first            = subDir.second             = g_settings->value("General/subtitle_dir").toString();
    subFontFamily.first     = subFontFamily.second      = g_settings->value("General/subtitle_font_family").toString();
    subFontSize.first       = subFontSize.second        = g_settings->value("General/subtitle_font_size").toInt();

    audioOut.first          = audioOut.second           = g_settings->value("General/audio_output_device").toString();
    globalVolume.first      = globalVolume.second       = g_settings->value("General/global_volume").toBool();
    standardVolume.first    = standardVolume.second     = g_settings->value("General/standard_volume").toBool();

    videoDecoder.first = videoDecoder.second = g_settings->value("General/video_decoder").toString();
    videoOutput.first = videoOutput.second = g_settings->value("General/video_output").toString();
    if (videoDecoder.first == "") {
        HardwareDecodecType decodeType = getHardwareDecodingType();
        if (decodeType == AMD_SOFTWARE) {
            videoOutput.second = tr("auto");
            videoDecoder.second = "no";
        }
        else if (decodeType == AMD_VDPAU) {
            videoOutput.second = "gpu";
            videoDecoder.second = "vdpau";
        }
        else if (decodeType == AMD_VAAPI) {
            videoOutput.second = "vaapi";
            videoDecoder.second = "vaapi";
        }
        else if (decodeType == INTEL_IrisXe_VAAPI) {
            videoOutput.second = "gpu";
            videoDecoder.second = "vaapi";
        }
        else if (decodeType == MooreThreads_VAAPI) {
            videoOutput.second = "gpu";
            videoDecoder.second = "vaapi";
        }
        else if (decodeType == ZDE_VDPAU_WID) {
            videoOutput.second = "vdpau";
            videoDecoder.second = "vdpau";
        }
        else if (decodeType == ZDE_VDPAU) {
            videoOutput.second = tr("auto");
            videoDecoder.second = "vdpau";
        }
        else if (decodeType == ZDE_VAAPI) {
            videoOutput.second = "vaapi";
            videoDecoder.second = "vaapi";
        }
        else if (decodeType == GlenFly_VAAPI) {
            videoOutput.second = "vaapi";
            videoDecoder.second = "vaapi";
        }
        else if (decodeType == JM9X_VAAPI) {
            if (Functions::isQingsongDevice()) {
                videoOutput.second = tr("auto");
                videoDecoder.second = "no";
            }
            else {
                videoOutput.second = "vaapi";
                videoDecoder.second = "vaapi";
            }
        }
        else if (decodeType == JM7X_VDPAU) {
            if (Functions::isQingsongDevice()) {
                videoOutput.second = tr("auto");
                videoDecoder.second = "no";
            }
            else {
                videoOutput.second = "vdpau";
                videoDecoder.second = "vdpau";
            }
        }
        else if (decodeType == Sm768_SOFTWARE) {
            videoOutput.second = "xv";
            videoDecoder.second = "no";
        }
        else if (decodeType == GP101_SOFTWARE) {
            videoOutput.second = "x11";
            videoDecoder.second = "no";
        }
        else if (decodeType == GPU_709_VAAPI) {
            videoOutput.second = "vaapi";
            videoDecoder.second = "vaapi";
        }
        else if (decodeType == Nvidia_VDPAU_COPY) {
            videoOutput.second = tr("auto");
            videoDecoder.second = "vdpau-copy";
        }
        else if (decodeType == X100_GPU) {
            videoOutput.second = "gpu";
            videoDecoder.second = tr("default");
        }
        else {
            videoOutput.second = tr("auto");
            videoDecoder.second = tr("default");
#if defined(__loongarch__)
            videoOutput.second = "x11";
            videoDecoder.second = "no";
#endif
#ifdef __mips64
            videoOutput.second = "x11";
            videoDecoder.second = "no";
#endif
            if (Functions::isVirtualSuppotGpu()) {
                videoOutput.second = "x11";
                videoDecoder.second = "no";
            }
        }
    }
    videoDecodeThreads.first = videoDecodeThreads.second = g_settings->value("General/video_decode_threads").toInt();
    if (videoDecodeThreads.first == 0) {
#if defined(__loongarch__)
        videoDecodeThreads.second = sysconf(_SC_NPROCESSORS_ONLN) * 2 + 1;
#else
        int hardwareMaxThread = std::thread::hardware_concurrency();
        videoDecodeThreads.second = hardwareMaxThread == 0 ? 4 : hardwareMaxThread;
#endif
    }

    seamlessBrowsing.first = seamlessBrowsing.second = g_settings->value("General/seamless_browsing").toBool();
    audioChannel.first = audioChannel.second = g_settings->value("General/audio_channel").toInt();
    flushChange();
}
