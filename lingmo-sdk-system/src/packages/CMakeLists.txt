aux_source_directory(. SOURCESCODE)
include_directories(.)
add_library(kypackage SHARED ${SOURCESCODE})
set_target_properties(kypackage PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(kypackage-test test/kypackage-test.c)
target_link_libraries(kypackage-test kypackage)

install(TARGETS kypackage
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkypackages.h
        DESTINATION include/lingmosdk/lingmosdk-system)