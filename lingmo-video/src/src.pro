QT += core gui sql x11extras dbus KWaylandClient gui-private av avwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lingmo-video
CONFIG += c++11 link_pkgconfig
MOC_DIR += .moc
PKGCONFIG += gsettings-qt wayland-client mpv lingmosdk-qtwidgets lingmosdk-sysinfo lingmosdk-waylandhelper
TRANSLATIONS += ./translations/lingmo-video_zh_CN.ts \
                ./translations/lingmo-video_bo_CN.ts \
                ./translations/lingmo-video_mn.ts

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS _UNICODE

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/KF5/KWindowSystem/ ./log ./sliderbar ./kwidget

!system($$PWD/translations/generate_translations_pm.sh): error("Failed to generate pm")
qm_files.files = translations/*.qm
qm_files.path = /usr/share/lingmo-video/translations/

target.path = /usr/bin
target.source = ./lingmo-video

desktop.path = /usr/share/applications/
desktop.files = ../lingmo-video.desktop

help_files.files = ../data/lingmo-video/
help_files.path = /usr/share/lingmo-user-guide/data/guide/

INSTALLS += target \
            desktop \
            qm_files \
            help_files

include(core/core.pri)
include(global/global.pri)
include(widget/widget.pri)

LIBS += -lX11 \
        -lKF5WindowSystem \
        -lzen \
        -lmediainfo \
        -llingmo-log4qt \
        -lQtAV \
        -lQtAVWidgets

RESOURCES += \
    resource/res.qrc

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

