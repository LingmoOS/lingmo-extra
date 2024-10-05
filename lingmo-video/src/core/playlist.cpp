#include "playlist.h"

#include <QFileInfo>
#include <QDateTime>
#include <QProcess>
#include <QVector>
#include <QThread>
#include <QTimer>
#include <QUuid>
#include <QMap>

#include <QDebug>
#include <lingmo-log4qt.h>

#include "core/mediahandle.h"
#include "global/globalsignal.h"
#include "global/global.h"
#include "global/path.h"

using namespace Global;

bool markSort(MarkItem &a, MarkItem &b) {
    if (a.m_filePath != b.m_filePath) {
        return a.m_filePath < b.m_filePath;
    }
    return a.m_markPos < b.m_markPos;
}

PlayList::PlayList(QObject *parent) : QObject(parent),
    m_lastFileChange(0)
{
    m_needPlayFile = "";
    connect(this, &PlayList::sigAddItem, this, &PlayList::addItem, Qt::BlockingQueuedConnection);
    initGlobalSig();
}

PlayList::~PlayList()
{

}

/** ************************************************************************
 * 添加播放列表条目
    initGlobalSig();
 * @param:
 *      file            :文件全路径
 *      duration        :媒体时长
 *      viewPath        :预览文件路径
 *      lastStopTime    :上次停止时间
 *      writeToDatabase :是否写如数据库
 *                        (添加新文件的时候需要写入数据库，初始化的时候不用)
 ***************************************************************************/
void PlayList::addItem(QString file, int duration, int lastStopTime, QString viewPath, bool writeToDatabase)
{
    QString t_name;
    bool    t_isExist;
    int     t_index;

    // 是否需要写入数据库
    if(writeToDatabase)
    {
        g_sqlite->insertPlayList(file, file.split("/").last(), duration, viewPath);
    }

    QString mark_character = viewPath.split("/").last();
    mark_character.chop(4);
    mark_character.remove(0, 1);

    t_name = file.split("/").last();

    PlayItem pi;
    {
        std::lock_guard<std::mutex> lg(m_mux);
        m_fileList << file;

        // 判断文件是否存在
        QFileInfo t_fi(file);
        if(t_fi.exists())
        {
            t_isExist   = true;
            t_index     = m_fileList.size()-1;
            if (m_availableIndexVec.indexOf(m_fileList.size()-1) < 0)
                m_availableIndexVec.push_back(m_fileList.size()-1);
        }
        else
        {
            t_isExist   = false;
            t_index     = -1;
            m_notExitFiles.insert(file);
        }

        pi.m_isExit         = t_isExist;
        pi.m_isNewForDB     = writeToDatabase;
        pi.m_fileName       = t_name;
        pi.m_mark           = mark_character;
        pi.m_indexOfList    = t_index;
        pi.m_filePath       = file;
        pi.m_duration       = duration;
        pi.m_previewPath    = viewPath;
        pi.m_lastStopTime   = lastStopTime;

        m_pathItemMap[file] = pi;
    }

    // 告诉界面添加了一个 item
    emit itemAdded(pi);
    if(m_needPlayFile == file)
    {
        g_user_signal->stop();
        g_user_signal->open(m_needPlayFile, 0);
    }
}

/** ************************************************************************
 * 删除播放列表条目
    initGlobalSig();
 * @param:
 *      file    :要删除的文件
 *      删除     :数据库、预览文件、临时map
 ***************************************************************************/
void PlayList::deleteFile(QString file)
{
    m_allFile.remove(file);
    if(m_pathItemMap.find(file) != m_pathItemMap.end())
    {
        // 删除预览文件
        QString cmd_ = QString("rm -rf ").append(m_pathItemMap[file].m_previewPath);
        system(cmd_.toStdString().c_str());

        {
            // 删除的时候锁一下
            std::lock_guard<std::mutex> lg(m_mux);
            // 如果是当前播放的文件，直接播放下一首
            m_pathItemMap.remove(file);
            for(int i=0; i<m_fileList.size(); i++)
            {
                if(m_fileList.at(i) == file)
                {
                    m_fileList.removeAt(i);
                    // 删除可用索引，之后全部可用索引-1
                    int t_rindex = -1;
                    for(int j=0; j<m_availableIndexVec.size(); j++)
                    {
                        if(m_availableIndexVec.at(j) == i)
                            t_rindex = j;
                        else if(m_availableIndexVec.at(j) > i)
                            m_availableIndexVec.replace(j, m_availableIndexVec.at(j)-1);
                    }
                    if(t_rindex >= 0)
                        m_availableIndexVec.remove(t_rindex);
                    break;
                }
            }
            // 从数据库中删除
            g_sqlite->deletePlayList(file);
            emit itemDelete(file);

            // 删除列表中书签内容
            QString markPath = "";
            for (int i=m_bookmarks.size()-1; i>=0; i--) {
                if (m_bookmarks.at(i).m_filePath == file) {
                    if (markPath == "") {
                        QStringList sl = m_bookmarks.at(i).m_previewPath.split("/");
                        markPath = sl.at(sl.size()-2);
                    }
                    m_bookmarks.remove(i);
                    continue;
                }
            }
            emit sigMarkClear();
            emit sigMarkUpdate(m_bookmarks);
            // 删除书签文件夹
            if (markPath != "") {
                QString cmd = QString("rm -rf %1/%2").arg(Paths::configPath()).arg(markPath);
                qDebug() << cmd;
                QProcess::execute(cmd);
            }

            // 解决 bug 110610 （之前在最前面）
            if(file == m_playFile) {
                if (m_availableIndexVec.size() >= 1 && g_playstate > 0) {
                    m_playIndex--;
                    g_user_signal->playNext(true);
                }
                else {
                    g_user_signal->stop();
                    // 没有可以播放的文件后就显示主页
                    g_user_signal->showStopFrame();
                }
            }
        }
    }
}

/** ***********************************************
 * 删除文件不存在的项
 * 说明：
 *      遍历所有项，判断文件是否存在，如果不存在的话删除
 **************************************************/
void PlayList::deleteInvalidItems()
{
    QStringList tmp_list(m_fileList);
    for (int i=0; i<m_fileList.size(); i++) {
        if (m_availableIndexVec.indexOf(i) < 0) {
            QString file = tmp_list.at(i);
            QThread::create([this, file](){
                this->deleteFile(file);
            })->start();
        }
    }
}

/** **********************************************
 * 清空列表
 * 说明：清空列表和数据库内容
 *************************************************/
void PlayList::clearList()
{
    // 清空数据库中数据
    g_sqlite->clearPlayList();
    m_availableIndexVec.clear();
    QStringList t_tmp_list(m_fileList);
    foreach (QString t_file, t_tmp_list) {
        emit itemDelete(t_file);
        if(m_pathItemMap.find(t_file) != m_pathItemMap.end())
        {
            // 删除预览文件
            QString cmd_ = QString("rm -rf ").append(m_pathItemMap[t_file].m_previewPath);
            system(cmd_.toStdString().c_str());
        }
    }
    m_pathItemMap.clear();
    m_fileList.clear();
    m_allFile.clear();
    m_bookmarks.clear();
    emit sigMarkClear();

    g_user_signal->stop();
    // 列表清空之后需要显示默认界面
    g_user_signal->showStopFrame();
}

/** **********************************************
 * 更新播放索引
 * 说明：播放列表内容改变索引
 *************************************************/
void PlayList::updateIndex()
{
    // 通过正在播放的文件全路径获取播放索引
    // 如果没有找到的话将索引设置为-1
    m_playIndex = -1;
    int i = 0;
    for(; i<m_fileList.size(); i++)
    {
        if(m_fileList.at(i) == m_playFile)
        {
            break;
        }
    }
    setPlayIndex(i >= m_fileList.size() ? -1 : i);
}

/** **********************************************
 * 设置当前索引
 * @param: _index 索引
 *************************************************/
void PlayList::setPlayIndex(int _index)
{
    if(m_availableIndexVec.size() == 0)
        return;
    if(m_availableIndexVec.back() < _index)
        return;
    m_playIndex = _index;
    emit sigIndexChange(m_playIndex);
    g_settings->setValue("History/playlist_index", m_playIndex);
}

/** **********************************************
 * 添加文件
 * @param: files 要添加的文件
 *************************************************/
void PlayList::addFiles(QStringList files)
{
    Extensions ext;
    QRegExp regexp(ext.multimedia().forRegExp());
    regexp.setCaseSensitivity(Qt::CaseInsensitive);

    // 修复一直往里拖动播放问题 ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
    QStringList valiableFiles = files;

    // 如果是无痕浏览的话不要添加到数据库和界面.
    if (g_config->seamlessBrowsing.first && files.size() > 0)
    {
        g_user_signal->stop();
        g_user_signal->open(files.first(), 0);
        return;
    }

    foreach (QString filename, files) {
        if (m_allFile.find(filename) != m_allFile.end())
            valiableFiles.removeOne(filename);
        else
            m_allFile.insert(filename);
    }
    if (valiableFiles.size() <= 0) {
        g_user_signal->stop();
        g_user_signal->open(files.first(), 0);
        return;
    }
    // 修复一直往里拖动播放问题 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

    // 添加后播放第一个添加的文件
    m_needPlayFile = valiableFiles.first();

    foreach (QString filename, valiableFiles) {
        QFileInfo fileInfo(filename);
        if (!fileInfo.isDir()) {
            if (regexp.indexIn(fileInfo.suffix()) > -1) {
                // 如果列表存在的话不重复添加
                if (m_pathItemMap.find(filename) != m_pathItemMap.end())
                {
                    if (!m_pathItemMap[filename].m_isExit)
                    {
                        m_pathItemMap[filename].m_isExit = true;
                        setAvaliable(filename);
                    }
                    if (m_needPlayFile == filename)
                    {
                        g_user_signal->stop();
                        g_user_signal->open(filename, 0);
                    }
                    continue;
                }
                // 添加文件后需要获取媒体时长，如果是第一个文件的话需要播放
                newFile(filename);
            }
        }
    }
}

/** **********************************************
 * 添加文件夹
 * 说明：非递归添加，只添加符合规则的媒体文件
 * @param: dir 要添加的文件夹
 *************************************************/
void PlayList::addDirectory(QString dir)
{
    m_needPlayFile = "";
    QStringList dirFiles;
    QString fullItemName;
    Extensions ext;
    QRegExp regexp(ext.multimedia().forRegExp());
    regexp.setCaseSensitivity(Qt::CaseInsensitive);

    QStringList dirEntryList = QDir(dir).entryList();
    QStringList::Iterator itemIterator = dirEntryList.begin();

    while(itemIterator != dirEntryList.end())
    {
        fullItemName = dir;
        if (fullItemName.right(1)!="/")
        {
            fullItemName += "/";
        }
        fullItemName += (*itemIterator);
        QFileInfo fileInfo(fullItemName);
        if (!fileInfo.isDir()) {
            if (regexp.indexIn(fileInfo.suffix()) > -1)
            {
                dirFiles.push_back(fullItemName);
            }
        }
        ++itemIterator;
    }
    if (dirFiles.size() != 0)
        addFiles(dirFiles);
}

/** **********************************************
 * 播放下一个(设置手动最快300ms切换一次)
 * 说明：播放下一个具体文件依据播放顺序和播放列表
 * @param : manual 如果是手动的话直接按列表循环播放下一个
 *************************************************/
void PlayList::playNext(bool manual)
{
    // 无痕浏览不能播放下一首，并且清空列表
    if (!manual && (g_config->seamlessBrowsing.first || m_availableIndexVec.size() == 0)) {
        clearList();
        return;
    }
    if (m_availableIndexVec.size() == 0 || (QDateTime::currentMSecsSinceEpoch() - m_lastFileChange < 300 && manual)) {
        KyInfo() << " available index size " << m_availableIndexVec.size();
        return;
    }
    if(m_playOrder == ONE_LOOP)
    {
        if (m_availableIndexVec.indexOf(m_playIndex) < 0)
            manual = true;
        if (manual) {
            do
            {
                if(m_availableIndexVec.indexOf(++m_playIndex) >= 0)
                    break;
            } while(m_playIndex < m_availableIndexVec.last());
            m_playIndex = m_playIndex > m_availableIndexVec.last() ? m_availableIndexVec.first() : m_playIndex;
        }
    }
    else
    {
        switch (m_playOrder) {
        case ONE_LOOP:
            m_playIndex = m_playIndex < 0 ? 0 : m_playIndex;
            break;
        case SEQUENCE:
            // 顺序播放，只播放一遍
            if(m_playIndex == m_availableIndexVec.last())
                // 停止播放
                return;
            do
            {
                if(m_availableIndexVec.indexOf(++m_playIndex) >= 0)
                    break;
            }while(m_playIndex < m_availableIndexVec.last());
            break;
        case LIST_LOOP:
            do
            {
                if(m_availableIndexVec.indexOf(++m_playIndex) >= 0)
                    break;
            }while(m_playIndex < m_availableIndexVec.last());
            m_playIndex = m_playIndex > m_availableIndexVec.last() ? m_availableIndexVec.first() : m_playIndex;
            break;
        case RANDOM:
            srand((unsigned int)time(0));
            m_playIndex = m_availableIndexVec.at(rand() % m_availableIndexVec.size());
            break;
        default:
            break;
        }
    }
    KyInfo() << " play index " << m_playIndex;
    if(m_fileList.size() > 0 && m_playIndex >= 0 && m_playIndex < m_fileList.size())
    {
        QString t_file_path = m_fileList.at(m_playIndex);

        g_user_signal->stop();
        g_user_signal->open(t_file_path, 0);
    }
    m_lastFileChange = QDateTime::currentMSecsSinceEpoch();
}

/** **********************************************
 * 播放上一个
 *************************************************/
void PlayList::playPrev(bool manual)
{
    if (m_availableIndexVec.size() == 0 || QDateTime::currentMSecsSinceEpoch()-m_lastFileChange < 300)
        return;
    if(manual && m_playOrder == ONE_LOOP)
    {
        do
        {
            if(m_availableIndexVec.indexOf(--m_playIndex) >= 0)
                break;
        }while(m_playIndex > m_availableIndexVec.first());
        m_playIndex = m_playIndex < m_availableIndexVec.first() ? m_availableIndexVec.last() : m_playIndex;
    }
    else
    {
        switch (m_playOrder) {
        case ONE_LOOP:
            break;
        case SEQUENCE:
            // 顺序播放，只播放一遍
            if(m_playIndex == m_availableIndexVec.first())
                // 停止播放
                return;
            do
            {
                if(m_availableIndexVec.indexOf(--m_playIndex) >= 0)
                    break;
            }while(m_playIndex > m_availableIndexVec.first());
            break;
        case LIST_LOOP:
            do
            {
                if(m_availableIndexVec.indexOf(--m_playIndex) >= 0)
                    break;
            }while(m_playIndex > m_availableIndexVec.first());
            m_playIndex = m_playIndex < m_availableIndexVec.first() ? m_availableIndexVec.last() : m_playIndex;
            break;
        case RANDOM:
            srand((unsigned int)time(0));
            m_playIndex = m_availableIndexVec.at(rand() % m_availableIndexVec.size());
            break;
        default:
            break;
        }
    }
    KyInfo() << " play index " << m_playIndex;
    if(m_fileList.size() > 0 && m_playIndex >= 0 && m_playIndex < m_fileList.size())
    {
        QString filePath = m_fileList.at(m_playIndex);
        g_user_signal->stop();
        g_user_signal->open(filePath, 0);
    }
    m_lastFileChange = QDateTime::currentMSecsSinceEpoch();
}

void PlayList::setNotAvaliable(QString file)
{
    if (m_pathItemMap.find(file) != m_pathItemMap.end()) {
        // 当前只是把不可用认为是不存在
        m_pathItemMap[file].m_isExit = false;
    }
    for (int i=0; i<m_fileList.size(); i++)
        if(m_fileList.at(i) == file)
            for(int j=0; j<m_availableIndexVec.size(); j++)
                if(m_availableIndexVec.at(j) == i)
                    m_availableIndexVec.remove(j);
}

void PlayList::setAvaliable(QString file)
{
    for(int i=0; i<m_fileList.size(); i++) {
        if(m_fileList.at(i) == file) {
            if (m_availableIndexVec.indexOf(i) < 0)
                m_availableIndexVec.push_back(i);
            qSort(m_availableIndexVec.begin(), m_availableIndexVec.end());
        }
    }
}

/** **********************************************
 * 添加书签
 * @param:  file 要添加的书签文件路径
 *          pos  书签位置秒
 *          desc 书签描述
 *          view 书签预览图
 *************************************************/
void PlayList::addMark(QString file, int pos, QString desc, QString view)
{
    // 如果书签存在的话就不要添加了
    for (MarkItem it : m_bookmarks) {
        if (it.m_filePath == file && it.m_markPos == pos) {
            return;
        }
    }

    g_sqlite->insertBookMark(file, m_duration, pos, desc, view);

    MarkItem item;
    item.m_filePath = file;
    item.m_duration = m_duration;
    item.m_markPos = pos;
    item.m_describe = desc;
    item.m_previewPath = view;
    item.m_fileExit = true;

    m_bookmarks.push_back(item);
    qSort(m_bookmarks.begin(), m_bookmarks.end(), markSort);
    emit sigMarkUpdate(m_bookmarks);
    m_playingFileMarks.push_back(item);
    emit sigInsertMark(item);
}

void PlayList::deleteMark(QString file, int pos)
{
    g_sqlite->deleteBookMark(file, pos);
    for (int i=0; i<m_bookmarks.size(); i++)
    {
        if (m_bookmarks.at(i).m_filePath == file && m_bookmarks.at(i).m_markPos == pos) {
            emit sigDeleteMark(m_bookmarks.at(i).m_markPos);
            m_bookmarks.remove(i);
            break;
        }
    }
}

void PlayList::slotFileInfoChange(Mpv::FileInfo _fi)
{
    if(_fi.file_path == QString())
    {
        return;
    }

    if(g_config->seamlessBrowsing.first)
    {
        m_playFile = _fi.file_path;
        updateIndex();
        // 如果是无痕浏览的话不要做添加操作,但是需要更新进度条上的书签
        goto update_mark;
    }
    m_duration = _fi.length;
    m_playFile = _fi.file_path;
    if(m_pathItemMap.find(m_playFile) == m_pathItemMap.end())
    {
        newFile(m_playFile);
    }
    updateIndex();

update_mark:
    // 文件改变之后书签列表重新排序
    // 文件改变之后需要刷新进度条书签
    m_playingFileMarks.clear();
    int index = -1, num = 0, i = 0;
    for (MarkItem item : m_bookmarks) {
        if (item.m_filePath == _fi.file_path) {
            m_playingFileMarks.push_back(item);
            if (index == -1)
                index = i;
            num++;
        }
        i++;
    }
    if (index >= 0)
        m_bookmarks.remove(index, num);
    qSort(m_bookmarks.begin(), m_bookmarks.end(), markSort);
    m_bookmarks = m_playingFileMarks + m_bookmarks;
    emit sigMarkUpdate(m_bookmarks);
    emit sigPlayingFileMarkUpdate(m_playingFileMarks);
}

void PlayList::initData()
{
    // 读取播放列表
    QVector<KPlaylistRecord> vecRec = g_sqlite->getPlayList();
    for(KPlaylistRecord rec : vecRec)
    {
        addItem(rec.path, rec.duration, rec.lastTime, rec.viewPath);
    }
    // 获取书签列表
    QVector<MarkRecord> vec_mark = g_sqlite->getMarks();
    for (MarkRecord mark : vec_mark) {
        MarkItem item;
        item.m_filePath = mark.path;
        item.m_duration = mark.duration;
        item.m_markPos = mark.pos;
        item.m_describe = mark.desc;
        item.m_previewPath = mark.preview;
        item.m_fileExit = (m_notExitFiles.find(mark.path) == m_notExitFiles.end());
        m_bookmarks.push_back(item);
    }
    qSort(m_bookmarks.begin(), m_bookmarks.end(), markSort);
    emit sigMarkUpdate(m_bookmarks);
    initIndex();
}


/** **********************************************
 * 新文件
 * 说明：新文件需要获取时长，然后添加至播放列表，如果文件获
 *      取内容失败需要做什么处理（当前未做处理）
 * @param: file 新文件绝对路径
 *************************************************/
void PlayList::newFile(QString file)
{
    // 预览关键帧保存为图片,名字为自动生成 uuid
    QUuid   uuid = QUuid::createUuid();
    QString name = uuid.toString().remove(0, 1);
    name.chop(1);
    QStringList sl = name.split("-");
    name = "";
    foreach (auto ts, sl) {
        name += ts;
    };

    QString filePath = Paths::previewPath().append("/").append(name.append(".jpg"));

    // 添加 item
    emit sigAddItem(file, 0, 0, filePath, true);
    QThread::create([this, file, filePath](){
        // 缩略图获取，执行命令获取缩略图
        QProcess p;
        p.start(QString("ffmpegthumbnailer -i %1 -o %2").arg("\"" + file + "\"").arg(filePath));
        p.waitForFinished();

        emit sigReloadView(file);
        // 时长获取
        KMedia::MediaHandle mediaHandle;
        mediaHandle.setFile(file);
        int duration = mediaHandle.getDuration()/1000.0;
        if (m_pathItemMap.find(file) != m_pathItemMap.end()) {
            m_pathItemMap[file].m_isNewForDB = false;
            m_pathItemMap[file].m_duration = duration;
        }
        if (duration > 0) {
            KyInfo() << "update list duration: " << file << " - " << duration;
            emit sigResetDuration(file, duration);
            g_sqlite->updateDuration(file, duration);
        }
    })->start();
}

void PlayList::initIndex()
{
    // 获取配置文件中但前列表索引
    if(g_settings->contains("History/playlist_index"))
        m_playIndex = g_settings->value("History/playlist_index").toInt();
    else
        m_playIndex = -1;

    // 判断下文件是否存在，不存在就不要设置当前选中行了
    if (m_fileList.size() > 0 && m_playIndex >= 0 && m_fileList.size() > m_playIndex) {
        QFileInfo fi(m_fileList.at(m_playIndex));
        if (fi.exists())
            setPlayIndex(m_playIndex);
    }
}

void PlayList::initGlobalSig()
{
    // 修改播放列表 主要是删除要用
    connect(g_user_signal, &GlobalUserSignal::sigListItemChange, [&](QString _file, int _duration, bool _isAdd){
        if(_isAdd)
            addItem(_file, _duration, 0, " ");
        else
            deleteFile(_file);
    });
    connect(g_user_signal, &GlobalUserSignal::sigClearPlayList  , this, &PlayList::clearList);

    connect(g_user_signal, &GlobalUserSignal::sigAddFiles       , [this](QStringList files){
        QThread::create([this, files](){addFiles(files);})->start();
    });
    connect(g_user_signal, &GlobalUserSignal::sigAddDir         , [this](QString dir){
        QThread::create([this, dir](){addDirectory(dir);})->start();
    });
    connect(g_user_signal, &GlobalUserSignal::sigPlayNext       , this, &PlayList::playNext);
    connect(g_user_signal, &GlobalUserSignal::sigPlayPrev       , this, &PlayList::playPrev);

    connect(g_user_signal, &GlobalUserSignal::sigChangePlayOrder, [&](){
        m_playOrder = m_playOrder==RANDOM ? (PlayOrder)0 : (PlayOrder)((int)m_playOrder+1);
        g_user_signal->setPlayOrder(m_playOrder);
    });

    connect(g_user_signal, &GlobalUserSignal::sigPlayOrder, [&](PlayOrder _order){
        m_playOrder = _order;
        g_settings->setValue("General/play_order", (int)_order);
    });

    connect(g_core_signal, &GlobalCoreSignal::sigFileInfoChange , this, &PlayList::slotFileInfoChange);


    connect(g_core_signal, &GlobalCoreSignal::sigFileLoadedError, this, [this](QString file){
        int err_index = 0;
        foreach (auto path, m_fileList) {
            if (path == file) {
                m_playIndex = err_index;
                break;
            }
            err_index++;
        }
        playNext(false);
    });

    connect(g_core_signal, &GlobalCoreSignal::sigMarkAdded, this, &PlayList::addMark);
    connect(g_core_signal, &GlobalCoreSignal::sigDuration, this, [this](QString file, int duration){
        if ((m_playFile == file && duration <= m_duration))
            return;
        m_duration = duration;
        // 更新数据库中时长
        g_sqlite->updateDuration(file, duration);
        m_pathItemMap[file].m_isNewForDB = false;
        m_pathItemMap[file].m_duration = duration;
        QTimer::singleShot(500, [this, file, duration]{
            // 延迟更新界面时间
            emit sigResetDuration(file, duration);
        });
#if 0
        if (duration == 0 && m_pathItemMap[file].m_duration != 0) {
            // 更新数据库中时长
            g_sqlite->updateDuration(file, duration);
            m_pathItemMap[file].m_isNewForDB = false;
            m_pathItemMap[file].m_duration = duration;
            emit sigResetDuration(file, duration);
        }
        if (m_pathItemMap.find(file) != m_pathItemMap.end()) {
            // 每次都更新
//            if (m_pathItemMap[file].m_isNewForDB) {
                // 更新数据库中时长
                g_sqlite->updateDuration(file, duration);
                m_pathItemMap[file].m_isNewForDB = false;
                m_pathItemMap[file].m_duration = duration;
                emit sigResetDuration(file, duration);
//            }
        }
#endif
    });
}

#if 0
// 添加一个书签，添加完成之后排序然后重新加载界面
MarkList::MarkList(const MarkList &other)
{
    m_fileName = other.m_fileName;
    m_duration = other.m_duration;
    m_itemMap = other.m_itemMap;
}

void MarkList::addItem(int pos, QString desc, QString view)
{
    MarkItem item;
    item.m_duration = m_duration;
    item.m_filePath = m_fileName;
    item.m_previewPath = view;
    item.m_describe = desc;
    item.m_markPos = pos;

    m_itemMap.insert(pos, item);
    emit sigUpdateUI(m_itemMap);
}

void MarkList::deleteItem(int pos)
{
    // 应该删除之后再去通知界面
    if (m_itemMap.find(pos) != m_itemMap.end()) {
        m_itemMap.remove(pos);
    }
}

MarkList &MarkList::operator =(const MarkList other)
{
    m_fileName = other.m_fileName;
    m_duration = other.m_duration;
    m_itemMap = other.m_itemMap;
    return *this;
}
#endif
