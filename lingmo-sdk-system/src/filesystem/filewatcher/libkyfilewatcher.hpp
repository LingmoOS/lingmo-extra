#ifndef KDK_SYSTEM_FILEWATCHER_H__
#define KDK_SYSTEM_FILEWATCHER_H__

#include "libkyfilewatcher_global.hpp"
#include <QObject>
#include <QHash>
#include <QThread>
#include <QFile>
#include <mutex>
#include <sys/inotify.h>

namespace KYSDK_FILEWATCHER {

enum FileWatcherType{
    ONESHOT = 0,    // 仅报送一次事件。请注意事件发生后，监听符号会从监听列表中移除，下次再用需要重新add
    PERIODIC,
    NEVER
};

enum FileWatcherAttribute{
    ACCESS = IN_ACCESS,
    CREATE = IN_CREATE,
    MODIFY = IN_MODIFY,
    DELETE = IN_DELETE | IN_DELETE_SELF,
    ATTRIB = IN_ATTRIB,
    MOVE   = IN_MOVE | IN_MOVE_SELF,
    OPEN   = IN_OPEN,
    CLOSE  = IN_CLOSE,
    ALL = IN_ALL_EVENTS
};

enum RecursiveType{
    DIR = 1 << 0,
    REGULAR = 1 << 1,
    HIDDEN = 1 << 2,
    TMPFILE = 1 << 3,    //认为那些以~开头的是临时文件
    ALLFILE = HIDDEN | TMPFILE | DIR | REGULAR
};

class FileWatcher;
extern void* fileWatcherThreadRunner(void *inst);

class FILEWATCHER_EXPORT FileWatcher : public QObject
{
    Q_OBJECT
public:
    FileWatcher();
    ~FileWatcher();

    /**
     * @brief 增加需要监听的文件路径，对于目录，默认启用递归监听子目录
     * @param url
     */
    int addWatchTarget(QString url, FileWatcherType type = PERIODIC, int attr = OPEN|CLOSE|MODIFY|DELETE);

    QStringList addWatchTargetRecursive(QString url, FileWatcherType type = PERIODIC, int attr = OPEN|CLOSE|MODIFY|DELETE, int maxdepth = 5, int recurType = DIR|REGULAR);

    /**
     * @brief 增加需要监听的文件列表
     * @param urlList
     */
    QStringList addWatchTargetList(QStringList urlList, FileWatcherType type = PERIODIC, int attr = OPEN|CLOSE|MODIFY|DELETE);

    QStringList addWatchTargetListRecursive(QStringList urlList, FileWatcherType type = PERIODIC, int attr = OPEN|CLOSE|MODIFY|DELETE, int maxdepth = 5, int recurType = DIR|REGULAR);

    int updateWatchTargetAttribute(QString url, int newattr);

    int updateWatchTargetType(QString url, FileWatcherType newtype);

    /**
     * @brief 从监听列表中移除指定的文件
     * @param url
     */
    int removeWatchTarget(QString url);
    QStringList removeWatchTargetRecursive(QString url, int maxdepth = 5, int recurType = ALLFILE);

    /**
     * @brief 从监听列表中移除指定的文件列表
     * @param urlList
     */
    QStringList removeWatchTargetList(QString urlList);
    QStringList removeWatchTargetListRecursive(QString urlList, int maxdepth = 5, int recurType = ALLFILE);

    /**
     * @brief 清空监听列表
     */
    void clearWatchList();

    /**
     * @brief 暂停文件监听
     */
    void pauseWatcher();

    /**
     * @brief 恢复文件监听
     */
    void restartWatcher();

    /**
     * @brief 获取文件监听列表
     * @return QStringList
     */
    QStringList getWatchList();
    QStringList getWatchList(FileWatcherType type);
    QStringList getWatchList(int attr);

Q_SIGNALS:
    void fileCreated(QString url, QString parent);
    void fileModified(QString url, QString parent);
    void fileDeleted(QString url, QString parent);
    void fileMoveOut(QString url, QString parent);
    void fileMoveIn(QString url, QString parent);
    void fileAccessed(QString url, QString parent);
    void fileOpened(QString url, QString parent);
    void fileClosed(QString url, QString parent);
    void fileAttrChanged(QString url, QString parent);
    void fileIgnoredAutomatic(QString url, QString parent);
    void fileUnmount(QString url, QString parent);

protected:
    void sendSignal(int wfd, QString name, int mask);

private:
    Q_DISABLE_COPY(FileWatcher)

    typedef struct {
        QString url;
        FileWatcherType type;
        int attr;
        int wfd;
    }FileDescription;
    std::mutex listLocker;
    std::mutex runningLocker;       // 控制暂停与继续监听变动
    QHash<QString, FileDescription> watchList;
    QHash<int, QString> fdCacheMap;
    int watcherFd;
    bool isActived;
    bool quit;
    pthread_t *threadRunner;

    friend void* fileWatcherThreadRunner(void *inst);
    int addWatchFile(FileDescription node);
    int removeWatchFile(QString url);
    int updateWatchFileAttribute(QString url, int attr);
    int updateWatchFileType(QString url, FileWatcherType type);
    QStringList getChildFile(QString parent, int depth, int maxdepth = 20, int recurType = REGULAR);
    QStringList getChildDir(QString parent, int depth, int maxdepth = 20, int recurType = DIR);
};

}

#endif // FILEWATCHER_H
