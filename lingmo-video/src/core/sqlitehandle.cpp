#include "sqlitehandle.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <lingmo-log4qt.h>

SqliteHandle* SqliteHandle::instance = nullptr;
static QSqlDatabase base;

SqliteHandle *SqliteHandle::getInstance(QString path)
{
    if(instance == nullptr)
    {
        if(path == "")
        {
            qDebug() << "first use plase set args";
            return nullptr;
        }
        instance = new SqliteHandle(path);
    }
    return instance;
}

/** *******************************
 * 获取播放列表
 * @return :
 *  <文件绝对路径 < 文件名, 时长 > >
 **********************************/
QVector<KPlaylistRecord> SqliteHandle::getPlayList()
{
    QVector<KPlaylistRecord> res;
    QSqlQuery sql_query(base);
    if(!sql_query.exec("select * from playlist"))
    {
        m_lastErrorMessage = sql_query.lastError().text();
    }
    else {
        while (sql_query.next()) {
            KPlaylistRecord rec;
            rec.path        = sql_query.value(0).toString();
            rec.name        = sql_query.value(1).toString();
            rec.duration    = sql_query.value(2).toInt();
            rec.mark        = sql_query.value(3).toString();
            rec.viewPath    = sql_query.value(4).toString();
            rec.lastTime    = sql_query.value(5).toInt();
            res.push_back(rec);
        }
    }
    return res;
}

/** *******************************
 * 插入一条数据到播放列表
 * 插入数据的时候对应建立书签数据表
 *
 * @param :
 *  path      : 文件绝对路径
 *  name      : 文件名
 *  duration  : 媒体时长
 **********************************/
bool SqliteHandle::insertPlayList(QString path, QString name, int duration, QString view)
{
    if(base.isOpen())
    {
        QString mark_table_name = view.split("/").last();
        mark_table_name.chop(4);
        QSqlQuery sql_query(base);
        if(!sql_query.exec(QString("insert into playlist values(\'%1\', \'%2\', %3, \'%5\', \'%4\', 0)")
                           .arg(path)
                           .arg(name)
                           .arg(duration)
                           .arg(view)
                           .arg("mark" + mark_table_name)))
        {
            qDebug() << "insert error " << sql_query.lastError();
            m_lastErrorMessage = sql_query.lastError().text();
            return false;
        }
    }
    return true;
}

/** *******************************
 * 更新最后播放时间（从上次停止的位置播放用）
 *
 * @param :
 *  path      : 文件绝对路径
 *  lasttime  : 停止时间
 **********************************/
bool SqliteHandle::updateLastTime(QString path, int lasttime)
{
    if(base.isOpen())
    {
        QSqlQuery sql_query(base);
        if(!sql_query.exec(QString("update playlist set lasttime=%0 where path=\'%1\'").arg(lasttime).arg(path)))
        {
            KyInfo() << sql_query.lastError();
            m_lastErrorMessage = sql_query.lastError().text();
            return false;
        }
    }
    return true;
}

bool SqliteHandle::updateDuration(QString path, int duration)
{
    if(base.isOpen())
    {
        QSqlQuery sql_query(base);
        if(!sql_query.exec(QString("update playlist set duration=%0 where path=\'%1\'").arg(duration).arg(path)))
        {
            KyInfo() << sql_query.lastError();
            m_lastErrorMessage = sql_query.lastError().text();
            return false;
        }
    }
    return true;
}

QString SqliteHandle::getMarkCharacter(QString path)
{
    if (!base.isOpen())
    {
        return QString();
    }

    QSqlQuery sql_query(base);
    if(!sql_query.exec(QString("select marktable from playlist where path=\'%1\'").arg(path)))
    {
        m_lastErrorMessage = sql_query.lastError().text();
        return QString();
    }
    if (sql_query.next())
        return sql_query.value(0).toString();
    else
        return QString();
}

/** *******************************
 * 插入书签
 * @param :
 *  path        : 文件绝对路径
 *  pos         : 书签位置（秒）
 *  desc        : 书签描述
 *
 * @return      : true/false 插入成功/失败
 **********************************/
bool SqliteHandle::insertBookMark(QString path, int duration, int pos, QString desc, QString view)
{
    if(base.isOpen())
    {
        QSqlQuery sql_query(base);
        // 执行插入操作
        if (!sql_query.exec(QString("insert into marklist values(\'%0\', %1, %2, \'%3\', \'%4\')").arg(path).arg(duration).arg(pos).arg(desc).arg(view))) {
            m_lastErrorMessage = sql_query.lastError().text();
            sql_query.clear();
            return false;
        }
        return true;
    }
    return false;
}

void SqliteHandle::deleteBookMark(QString path, int pos)
{
    if(base.isOpen())
    {
        QSqlQuery sql_query(base);
        sql_query.clear();
        // 执行删除操作
        if (!sql_query.exec(QString("delete from marklist where path=\'%1\' and pos=%2").arg(path).arg(pos))) {
            m_lastErrorMessage = sql_query.lastError().text();
            sql_query.clear();
        }
    }
}

QVector<MarkRecord> SqliteHandle::getMarks()
{
    QVector<MarkRecord> vec;
    if (base.isOpen()) {
        QSqlQuery sql_query(base);
        if (sql_query.exec(QString("select * from marklist"))) {
            while (sql_query.next()) {
                MarkRecord rec;
                rec.path = sql_query.value(0).toString();
                rec.duration = sql_query.value(1).toInt();
                rec.pos = sql_query.value(2).toInt();
                rec.desc = sql_query.value(3).toString();
                rec.preview = sql_query.value(4).toString();
                vec.push_back(rec);
            }
        }
    }
    return vec;
}

/** *******************************
 * 获取上次停止时间
 * @param :
 *  path        : 文件绝对路径
 *
 * @return      : 上次停止的时间
 **********************************/
int SqliteHandle::getLastTime(QString path)
{
    QSqlQuery sql_query(base);
    if(!sql_query.exec(QString("select duration,lasttime from playlist where path=\'%1\'").arg(path)))
    {
        qDebug() << sql_query.lastError();
        m_lastErrorMessage = sql_query.lastError().text();
    }
    else {
        while (sql_query.next()) {
            // 如果停止时间大于等于总时长或者只剩下一秒（因为有的视频播放不到最后一秒，不知道为啥），则返回跳转时间为 0
            return sql_query.value(1).toInt() >= (sql_query.value(0).toInt()-1) ? 0 : sql_query.value(1).toInt();
        }
    }
    return 0;
}

int SqliteHandle::getDuration(QString path)
{
    QSqlQuery sql_query(base);
    if(!sql_query.exec(QString("select duration from playlist where path=\'%1\'").arg(path)))
    {
        qDebug() << sql_query.lastError();
        m_lastErrorMessage = sql_query.lastError().text();
    }
    else {
        while (sql_query.next()) {
            // 如果停止时间大于等于总时长或者只剩下一秒（因为有的视频播放不到最后一秒，不知道为啥），则返回跳转时间为 0
            return sql_query.value(0).toInt();
        }
    }
    return 0;
}

/** *******************************
 * 删除播放列表中一条数据
 * 同时需要删除书签
 * @param :
 *  path      : 文件绝对路径
 **********************************/
bool SqliteHandle::deletePlayList(QString path)
{
    QSqlQuery sql_query(base);
    if (!sql_query.exec(QString("delete from marklist where path=\'%1\'").arg(path))) {
        // 删除书签失败
        m_lastErrorMessage = sql_query.lastError().text();
    }
    sql_query.clear();

    if (!sql_query.exec(QString("delete from playlist where path=\'%1\'").arg(path)))
    {
        m_lastErrorMessage = sql_query.lastError().text();
        return false;
    }
    return true;
}

/** *******************************
 * 清空播放列表
 **********************************/
bool SqliteHandle::clearPlayList()
{
    // 删除播放表和书签表
    QSqlQuery sql_query(base);
    if (!sql_query.exec(QString("delete from playlist"))) {
        KyInfo() << sql_query.lastError();
        m_lastErrorMessage = sql_query.lastError().text();
        return false;
    }
    if (!sql_query.exec(QString("delete from marklist"))) {
        KyInfo() << sql_query.lastError();
        m_lastErrorMessage = sql_query.lastError().text();
        return false;
    }
    return true;
}

SqliteHandle::SqliteHandle(QString path)
{
    base = QSqlDatabase::addDatabase("QSQLITE");
    base.setDatabaseName(path);
    if (!base.open())
    {
        KyInfo() << "Error: Failed to connect database." << base.lastError();
    }
    else
    {
        KyInfo() << "Succeed to connect database." ;
    }

    QSqlQuery sql_query(base);
    /** ********************************
     * 创建数据表
     * path     : 文件绝对路径
     * name     : 文件名
     * duration : 时长
     * marks    : 书签(mark1_mark2_mark3)
     * view     : 预览图片路径
     * lasttime : 上次停止时间
     ***********************************/
    if(!sql_query.exec("create table playlist(path text, name text, duration int, marktable text, view text, lasttime int)"))
    {
        KyInfo() << "Error: Fail to create table." << sql_query.lastError();
        m_lastErrorMessage = sql_query.lastError().text();
    }
    else
    {
        KyInfo() << "Table playlist created!";
    }

    sql_query.clear();
    /** ********************************
     * 创建书签表
     * path     : 文件绝对路径
     * pos      : 书签时间
     * describe : 书签描述
     * preview  : 预览图绝对路径
     ***********************************/
    if(!sql_query.exec("create table marklist(path text, int duration, pos int, describe text, preview text)"))
    {
        m_lastErrorMessage = sql_query.lastError().text();
    }
    else
    {
        KyInfo() << "Table marklist created!";
    }
}

SqliteHandle::~SqliteHandle()
{
    if(base.isOpen())
        base.close();
}
