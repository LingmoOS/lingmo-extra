QT += KWaylandClient KWaylandServer gui widgets x11extras KWindowSystem KIconThemes

TEMPLATE = lib
DEFINES += KYSDKWAYLANDHELPER_LIBRARY

CONFIG += c++11 link_pkgconfig
PKGCONFIG += wayland-client x11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/lingmostylehelper/lingmo-decoration-core.c \
    src/lingmostylehelper/lingmo-decoration-manager.cpp \
    src/lingmostylehelper/lingmostylehelper.cpp \
    src/lingmostylehelper/xatom-helper.cpp \
    src/waylandhelper.cpp \
    src/windowmanager/abstractinterface.cpp \
    src/windowmanager/waylandinterface.cpp \
    src/windowmanager/wmregister.cpp \
    src/windowmanager/xcbinterface.cpp \
    src/windowmanager/windowmanager.cpp \
    src/windowmanager/wayland-plasma-window-management-protocol.c

HEADERS += \
    src/lingmostylehelper/lingmo-decoration-client.h \
    src/lingmostylehelper/lingmo-decoration-manager.h \
    src/lingmostylehelper/lingmostylehelper.h \
    src/lingmostylehelper/xatom-helper.h \
    src/waylandhelper.h \
    src/windowmanager/abstractinterface.h \
    src/windowmanager/waylandinterface.h \
    src/windowmanager/windowinfo.h \
    src/windowmanager/wmregister.h \
    src/windowmanager/xcbinterface.h \
    src/windowmanager/windowmanager.h \
    src/windowmanager/wayland-plasma-window-management-client-protocol.h \
    src/lingmosdk-waylandhelper_global.h 

# Default rules for deployment.
headers.files = src/lingmosdk-waylandhelper_global.h \
                src/waylandhelper.h
headers.path = /usr/include/lingmosdk/applications/

wm_headers.files =  src/windowmanager/abstractinterface.h \
                    src/windowmanager/waylandinterface.h \
                    src/windowmanager/windowinfo.h \
                    src/windowmanager/wmregister.h \
                    src/windowmanager/xcbinterface.h \
                    src/windowmanager/windowmanager.h \
                    src/windowmanager/wayland-plasma-window-management-client-protocol.h 
wm_headers.path = /usr/include/lingmosdk/applications/windowmanager/

sty_headers.files = src/lingmostylehelper/lingmo-decoration-client.h \
                    src/lingmostylehelper/lingmo-decoration-manager.h \
                    src/lingmostylehelper/lingmostylehelper.h \
                    src/lingmostylehelper/xatom-helper.h

sty_headers.path = /usr/include/lingmosdk/applications/lingmostylehelper/

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target headers wm_headers sty_headers
