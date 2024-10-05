aux_source_directory(linklist/skip_linklist SOURCESCODESTRUCT)
add_library(kydatastruct SHARED ${SOURCESCODESTRUCT})
set_target_properties(kydatastruct PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_subdirectory(linklist)

include_directories(linklist)
include_directories(linklist/skip_linklist)

add_executable(test-delete linklist/skip_linklist/test/delete_test.c)
add_executable(test-insert linklist/skip_linklist/test/insert_test.c)
add_executable(test-search linklist/skip_linklist/test/search_test.c)
target_link_libraries(test-delete kydatastruct)
target_link_libraries(test-insert kydatastruct)
target_link_libraries(test-search kydatastruct)

install(TARGETS kydatastruct
        DESTINATION lib/lingmosdk/lingmosdk-base)