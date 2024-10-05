#ifndef SQLITEHANDLE_H
#define SQLITEHANDLE_H


#include <QString>
#include <QVector>

class QSqlDatabase;

class KPlaylistRecord
{
public:
    QString path;
    QString name;
    QString mark;
    QString viewPath;
    int lastTime;
    int duration;
};

struct MarkRecord {
    QString path;
    int     duration;
    int     pos;
    QString desc;
    QString preview;
};


class SqliteHandle
{
public:
    static SqliteHandle *getInstance(QString path = "");

    QVector<KPlaylistRecord> getPlayList();
    int     getLastTime     (QString path);
    int     getDuration     (QString path);
    bool    insertPlayList  (QString path, QString name, int duration, QString view);
    bool    updateLastTime  (QString path, int lasttime);
    bool    updateDuration  (QString path, int duration);
    bool    deletePlayList  (QString path);
    bool    clearPlayList   ();

    bool    insertBookMark  (QString path, int duration, int pos, QString desc, QString view);
    void    deleteBookMark  (QString path, int pos);
    QVector<MarkRecord> getMarks();
    QString getMarkCharacter(QString path);

    QString lastError       (){return m_lastErrorMessage;}

private:
    static SqliteHandle* instance;
    SqliteHandle(QString);
    ~SqliteHandle();

    QString m_lastErrorMessage;
};

#endif // SQLITEHANDLE_H
