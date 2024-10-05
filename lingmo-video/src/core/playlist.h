#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QSet>

#include <mutex>

#include "core/mpvtypes.h"
#include "global/globalsignal.h"

/** *********************************
 * 列表 : item 数据
 ************************************/
enum FileErrorState {
    None,
    NotExist,
    BadFile
};

class PlayItem
{
public:
    PlayItem():
        m_fileName(""),
        m_filePath(""),
        m_previewPath(""),
        m_isExit(true),
        m_isNewForDB(false),
        m_duration(-1),
        m_lastStopTime(-1),
        m_indexOfList(-1){}
    ~PlayItem(){}

    QString m_mark,         // 书签
            m_fileName,     // 文件名
            m_filePath,     // 文件路径
            m_previewPath;  // 预览文件路径

    FileErrorState m_errorState;

    bool    m_isExit,       // 文件是否存在
            m_isNewForDB,   // 是否第一次添加到数据库，如果是要更新媒体时长
            m_isNetStream;  // 是否网络流

    int     m_duration,     // 文件总秒数
            m_lastStopTime, // 最后停止时间
            m_indexOfList;  // 该条目在列表中的索引
};

class MarkItem {
public:
    MarkItem() {}
    ~MarkItem(){}

    QString m_filePath,
            m_describe,
            m_previewPath;

    int m_duration,
        m_markPos;

    bool m_fileExit;
};

#if 0
class MarkList : public QObject
{
    Q_OBJECT
public:
    MarkList(){}
    MarkList(QString file, int duration) :
        m_fileName(file),
        m_duration(duration){}

    MarkList(const MarkList &other);
    ~MarkList() {}

    void addItem(int pos, QString desc, QString view);
    void deleteItem(int pos);
    QMap<int, MarkItem> getMarkMap(){return m_itemMap;}

    inline MarkList &operator =(const MarkList other);

signals:
    void sigUpdateUI(QMap<int, MarkItem>);

private:
    int m_duration;
    QString m_fileName;
    QMap<int, MarkItem> m_itemMap;
};
#endif

class PlayList : public QObject
{
    Q_OBJECT
public:
    explicit PlayList(QObject *parent = nullptr);
    ~PlayList();
    void initData();
    int getSize(){return m_fileList.size();}
    QVector<MarkItem> getPlayingFileMarks(){return m_playingFileMarks;}

public slots:
    void addItem(QString _file, int _duration, int _lastTime, QString _viewPath, bool writeToDatabase = false);
    void deleteFile(QString _file);
    void deleteInvalidItems();
    void clearList();
    void updateIndex();
    void setPlayIndex(int _index);

    void addFiles(QStringList _files);
    void addDirectory(QString dir);
    void playNext(bool manual);
    void playPrev(bool manual);
    void setNotAvaliable(QString file);
    void setAvaliable(QString file);

    void addMark(QString file, int pos, QString desc, QString view);
    void deleteMark(QString file, int pos);

private slots:
    void slotFileInfoChange(Mpv::FileInfo _fi);

signals:
    void itemAdded(PlayItem);
    void itemDelete(QString);

    void sigAddItem(QString, int, int, QString, bool);
    void sigDeleteItem(QString);
    void sigIndexChange(int);
    void sigReloadView(QString);
    void sigResetDuration(QString, int);

    void sigMarkClear();
    void sigMarkUpdate(QVector<MarkItem>);
    void sigInsertMark(MarkItem);
    void sigDeleteMark(int);
    void sigPlayingFileMarkUpdate(QVector<MarkItem>);

private:
    std::mutex              m_mux;
    int                     m_playIndex;            // 当前播放索引
    int                     m_duration;             // 当前播放文件时长
    PlayOrder               m_playOrder;            // 播放顺序
    QString                 m_playFile,             // 当前播放文件
                            m_needPlayFile;         // 添加新文件后需要播放的文件
    QStringList             m_fileList;             // 播放列表
    QVector<int>            m_availableIndexVec;    // 可用播放索引 (如果文件不存在的话则不可用)
    QMap<QString, PlayItem> m_pathItemMap;          // <绝对路径, item 数据>
    QVector<MarkItem>       m_bookmarks;            // 所有书签
    QVector<MarkItem>       m_playingFileMarks;     // 正在播放的文件所有书签
    QSet<QString>           m_notExitFiles;         // 本地不存在文件
    QSet<QString>           m_allFile;              // 防止重复添加
    int64_t                 m_lastFileChange;       // 上次切换时间

    void newFile(QString file);
    void initIndex();
    void initGlobalSig();
};

Q_DECLARE_METATYPE(PlayItem)

#endif // PLAYLIST_H
