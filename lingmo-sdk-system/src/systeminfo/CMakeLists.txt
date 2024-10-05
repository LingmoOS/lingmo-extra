aux_source_directory(. SOURCESCODE)
include_directories(.)
include_directories(/usr/include/cjson/)
find_library(GLIBC_LIB glib-2.0)
find_library(DBUS_LIB dbus-1)
find_library(DBUS_GLIB_LIB dbus-glib-1)
find_package(PkgConfig)
add_library(kysysinfo SHARED ${SOURCESCODE})
set_target_properties(kysysinfo PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(kysysinfo-test test/kysysinfo_test.c)
target_link_libraries(kysysinfo dl kylog systemd kyconf X11 Xrandr libcjson.so pci kmod ${GLIBC_LIB} ${DBUS_LIB} ${DBUS_GLIB_LIB})
target_link_libraries(kysysinfo-test kysysinfo libcjson.so)
# target_link_libraries(kysysinfo-test kysysinfo lingmo-activation kylog systemd kyconf ${GLIBC_LIB} ${DBUS_LIB} ${DBUS_GLIB_LIB})

install(TARGETS kysysinfo
        DESTINATION lib/lingmosdk/lingmosdk-system)
        
install(FILES libkysysinfo.hpp
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkysysinfo.h
        DESTINATION include/lingmosdk/lingmosdk-system)
