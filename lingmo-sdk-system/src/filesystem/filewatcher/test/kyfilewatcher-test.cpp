#include <QCoreApplication>
#include <QDebug>
#include "../libkyfilewatcher.hpp"

using namespace KYSDK_FILEWATCHER;

int main(int argc, char *argv[])
{
    KYSDK_FILEWATCHER::FileWatcher watcher;
    watcher.addWatchTargetRecursive("/home/lingmo/", PERIODIC, ALL, 3, REGULAR);

    getchar();

//    qDebug() << "update attr test";
//    watcher.updateWatchTargetAttribute("/data/testcode/test.c", DELETE);

//    getchar();

//    qDebug() << "re-add test";
//    watcher.addWatchTarget("/data/testcode/test.c", PERIODIC, ALL);

//    getchar();

//    qDebug() << "update oneshot test";
//    watcher.updateWatchTargetType("/data/testcode/test.c", ONESHOT);

//    getchar();

//    qDebug() << "remove target test";
//    watcher.removeWatchTarget("/data/testcode/test.c");

//    getchar();

    watcher.clearWatchList();
    return 0;
}
