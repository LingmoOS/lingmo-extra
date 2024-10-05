aux_source_directory(. SOURCESCODE)
add_library(kyconf SHARED ${SOURCESCODE})
set_target_properties(kyconf PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(kyconf-test-struct test/test_structlist.c)
target_link_libraries(kyconf-test-struct kyconf kylog pthread)

install(TARGETS kyconf
        DESTINATION lib/lingmosdk/lingmosdk-base)

# install(FILES libkyconf.h
#         DESTINATION include/lingmosdk/lingmosdk-base)