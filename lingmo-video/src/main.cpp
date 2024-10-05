#include "mainwindow.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
#include <QIcon>

#include "mainwindow.h"
#include "global/translator.h"
#include "global/global.h"
#include "global/functions.h"
#include "global/globalconfig.h"

#include <lingmo-log4qt.h>

int main(int argc, char *argv[])
{
    initLingmoLog4qt("lingmo-video");
    Global::initGlobal();

    if(QString(qgetenv("XDG_SESSION_TYPE")) == "wayland")
    {
#if 0
        qputenv("QT_QPA_PLATFORM", "wayland");
#endif
    }
    else {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    }

    if (!Functions::isVirtualSuppotGpu() && GlobalConfig::getInstance()->hardwareType() != GlobalConfig::Nvidia_VDPAU_COPY)
        qputenv("QT_XCB_GL_INTEGRATION", "xcb_egl");

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon::fromTheme("lingmo-video"));

    // Translator
    Translator *translator = new Translator();
    translator->load("");

    // 加载 sdk 翻译
    QTranslator *tran = new QTranslator;
    if (QLocale::system().name() == "zh_CN")
        tran->load(":/translations/gui_zh_CN.qm");
    else if (QLocale::system().name() == "bo_CN")
        tran->load(":/translations/gui_bo_CN.qm");
    else if (QLocale::system().name() == "zh_HK")
        tran->load(":/translations/gui_bo_CN.qm");
    a.installTranslator(tran);

    QStringList arg_list;
    for (int i=1; i<argc; i++)
    {
        if (QString(argv[i]).startsWith("http"))
        {
            arg_list << QString(argv[i]);
            continue;
        }
        QFileInfo f(QString(argv[i]));
        if(f.isFile())
            arg_list << f.absoluteFilePath();
    }

    MainWindow w(arg_list);
    w.show();
    kdk::WindowManager::setGeometry(w.windowHandle(),
                                    QRect(qApp->desktop()->geometry().center() - w.geometry().center(),
                                    QSize(w.size())));

    return a.exec();
}
