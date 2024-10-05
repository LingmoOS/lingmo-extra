aux_source_directory(. SOURCESCODE)
add_library(kydiskinfo SHARED ${SOURCESCODE})
set_target_properties(kydiskinfo PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(test-getdiskinfo test/getdiskinfo.c)
add_executable(test-getdisklist test/getdisklist.c)
find_library(UDEV_LIB udev)
target_link_libraries(kydiskinfo blkid kylog pthread systemd kyconf hd ${UDEV_LIB})
target_link_libraries(test-getdiskinfo kydiskinfo)
target_link_libraries(test-getdisklist kydiskinfo)
# target_link_libraries(test-getdiskinfo kydiskinfo blkid kylog pthread systemd kyconf ${UDEV_LIB})
# target_link_libraries(test-getdisklist kydiskinfo blkid kylog pthread systemd kyconf ${UDEV_LIB})

install(TARGETS kydiskinfo
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkydiskinfo.h
        DESTINATION include/lingmosdk/lingmosdk-system)
