set (CMAKE_AUTOMOC ON)

aux_source_directory(. SOURCECODE)

find_package(Qt5Core)
include_directories(${Qt5Core_INCLUDE_DIRS})
add_library(kyfilewatcher SHARED ${SOURCECODE})
set_target_properties(kyfilewatcher PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(kyfilewatcher-test test/kyfilewatcher-test.cpp)
target_link_libraries(kyfilewatcher kylog kyconf systemd pthread ${Qt5Core_LIBRARIES})
target_link_libraries(kyfilewatcher-test kyfilewatcher)

install(TARGETS kyfilewatcher
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkyfilewatcher.hpp
        DESTINATION include/lingmosdk/lingmosdk-system)

install(FILES libkyfilewatcher_global.hpp
        DESTINATION include/lingmosdk/lingmosdk-system)