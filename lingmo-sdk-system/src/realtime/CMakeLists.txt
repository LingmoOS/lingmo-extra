aux_source_directory(. CODE)

add_library(kyrealtime SHARED ${CODE})

set_target_properties(kyrealtime PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_executable(kyrealtime-test test/kyrealtime-test.c)
target_link_libraries(kyrealtime-test kyrealtime)

add_executable(realtimeinfo-test test/realtimeinfo-test.cpp)
target_link_libraries(realtimeinfo-test kyrealtime)

install(TARGETS kyrealtime
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkyrealtimeinfo.h
        DESTINATION include/lingmosdk/lingmosdk-system)
