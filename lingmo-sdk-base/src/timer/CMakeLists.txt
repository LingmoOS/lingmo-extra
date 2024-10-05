aux_source_directory(. SOURCESCODE)
add_library(kytimer SHARED ${SOURCESCODE})
set_target_properties(kytimer PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(test-kytimer test/test-kytimer.c)
target_link_libraries(kytimer pthread)
target_link_libraries(test-kytimer kytimer)

install(TARGETS kytimer
        DESTINATION lib/lingmosdk/lingmosdk-base)

# install(FILES libkytimer.h
#         DESTINATION include/lingmosdk/lingmosdk-base)