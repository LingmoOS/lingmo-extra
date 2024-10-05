aux_source_directory(. SOURCESCODE)
include(CMakePackageConfigHelpers)
include(FindPkgConfig)
pkg_check_modules(DBus REQUIRED
                dbus-1)
include_directories(${DBus_INCLUDE_DIRS})
add_library(kylog SHARED ${SOURCESCODE})
set_target_properties(kylog PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(kylog-testlog test/test-log.c)
add_executable(kylog-testsetdir test/test-setdir.c)
add_executable(kylog-testpressure test/test-pressure.c)
add_executable(kylog-testautowrap test/test-autowrap.c)
find_library(SYSTEMD_LIB systemd)
find_library(DBUS_LIB dbus-1)
target_link_libraries(kylog kyconf pthread ${SYSTEMD_LIB} ${DBUS_LIB})
target_link_libraries(kylog-testlog kylog)
target_link_libraries(kylog-testsetdir kylog)
target_link_libraries(kylog-testautowrap kylog)
target_link_libraries(kylog-testpressure kylog)

install(TARGETS kylog
        DESTINATION lib/lingmosdk/lingmosdk-base)

install(FILES libkylog.h
        DESTINATION include/lingmosdk/lingmosdk-base)

install(FILES kylog-rotate-default
        DESTINATION /etc/lingmosdk/lingmosdk-base)