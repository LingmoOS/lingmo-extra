include(FindPkgConfig)
pkg_check_modules(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})

set(HARDWARE_TOP_DIR ${CMAKE_CURRENT_LIST_DIR})
set(hwcode 
    "${HARDWARE_TOP_DIR}/libkybios.h"
    "${HARDWARE_TOP_DIR}/libkybios.c"
    "${HARDWARE_TOP_DIR}/libkyboard.h"
    "${HARDWARE_TOP_DIR}/libkyboard.c"
    "${HARDWARE_TOP_DIR}/libkycpu.h"
    "${HARDWARE_TOP_DIR}/libkycpu.c"
    "${HARDWARE_TOP_DIR}/libkydisplay.h"
    "${HARDWARE_TOP_DIR}/libkydisplay.c"
    "${HARDWARE_TOP_DIR}/libkync.h"
    "${HARDWARE_TOP_DIR}/libkync.c"
    "${HARDWARE_TOP_DIR}/libkyprinter.h"
    "${HARDWARE_TOP_DIR}/libkyprinter.c"
    "${HARDWARE_TOP_DIR}/libkyusb.h"
    "${HARDWARE_TOP_DIR}/libkyusb.c")


set(bluetoothcode 
    "${HARDWARE_TOP_DIR}/libkybluetooth.h"
    "${HARDWARE_TOP_DIR}/libkybluetooth.c")

set(edidcode 
    "${HARDWARE_TOP_DIR}/libkyedid.h"
    "${HARDWARE_TOP_DIR}/libkyedid.c")

set(fancode 
    "${HARDWARE_TOP_DIR}/libkyfan.h"
    "${HARDWARE_TOP_DIR}/libkyfan.c")

set(hwinfocode 
    "${HARDWARE_TOP_DIR}/libkyhw.h"
    "${HARDWARE_TOP_DIR}/libkyhw.c")


add_library(kyhw SHARED ${hwcode})
set_target_properties(kyhw PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_library(kybluetooth SHARED ${bluetoothcode})
set_target_properties(kybluetooth PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_library(kyedid SHARED ${edidcode})
set_target_properties(kyedid PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_library(kyfan SHARED ${fancode})
set_target_properties(kyfan PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_library(kyhwinfo SHARED ${hwinfocode})
set_target_properties(kyhwinfo PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_executable(kync-test test/kync-test.c)
add_executable(kycpu-test test/kycpu-test.c)
add_executable(kyprinterlist-test test/kyprinterlist-test.c)
add_executable(kyprinterprint-test test/kyprinterprint-test.c)
add_executable(kyusb-test test/kyusb-test.c)
add_executable(kybios-test test/kybios-test.c)
add_executable(kyboard-test test/kyboard-test.c)
add_executable(kydisplay-test test/kydisplay-test.c)
add_executable(kyedid-test test/kyedid-test.c)
add_executable(kybluetooth-test test/kybluetooth-test.c)
add_executable(kyfan-test test/kyfan-test.c)
add_executable(kyhw-test test/kyhw-test.c)
# 额外链接的库在这一行
target_link_libraries(kyhw kylog kyconf pthread systemd cups curl udev ${GLIB_LIBRARIES})
target_link_libraries(kybluetooth bluetooth hd gobject-2.0)
target_link_libraries(kyedid kylog m X11 Xrandr hd)
target_link_libraries(kyfan sensors)
target_link_libraries(kyhwinfo kylog hd pci dbus-glib-1)
# target_link_libraries(kyhw kylog kyconf pthread systemd cups curl udev X11 Xrandr bluetooth sensors hd pci gobject-2.0 dbus-glib-1 ${GLIB_LIBRARIES})
target_link_libraries(kyprinterprint-test kyhw)
target_link_libraries(kyprinterlist-test kyhw)
target_link_libraries(kync-test kyhw)
target_link_libraries(kycpu-test kyhw)
target_link_libraries(kyusb-test kyhw)
target_link_libraries(kybios-test kyhw)
target_link_libraries(kyboard-test kyhw)
target_link_libraries(kydisplay-test kyhw)
target_link_libraries(kyedid-test kyedid)
target_link_libraries(kybluetooth-test kybluetooth)
target_link_libraries(kyfan-test kyfan)
target_link_libraries(kyhw-test kyhwinfo)

install(TARGETS kyhw
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(TARGETS kybluetooth
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(TARGETS kyedid
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(TARGETS kyfan
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(TARGETS kyhwinfo
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkync.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkycpu.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkyprinter.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkybios.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkyboard.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkyusb.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkydisplay.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkyedid.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkybluetooth.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkyfan.h
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkyhw.h
        DESTINATION include/lingmosdk/lingmosdk-system)
