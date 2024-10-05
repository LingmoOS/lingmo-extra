aux_source_directory(. CODE)
add_library(kynetinfo SHARED ${CODE})
set_target_properties(kynetinfo PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_executable(kynet-test test/kynet-test.c)
target_link_libraries(kynet-test kynetinfo)

install(TARGETS kynetinfo
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkynetinfo.h
        DESTINATION include/lingmosdk/lingmosdk-system)