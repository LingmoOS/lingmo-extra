aux_source_directory(. SOURCESCODE)
include_directories(.)
add_library(kyrtinfo SHARED ${SOURCESCODE})
set_target_properties(kyrtinfo PROPERTIES VERSION 2.0.0 SOVERSION 1)
add_executable(kyrtinfo-test test/kyrtinfo-test.c)
add_executable(kyprocess-test test/kyprocess-test.c)
target_link_libraries(kyrtinfo kylog)
target_link_libraries(kyrtinfo-test kyrtinfo) 
target_link_libraries(kyprocess-test kyrtinfo)

install(TARGETS kyrtinfo
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkyrtinfo.h
        DESTINATION include/lingmosdk/lingmosdk-system)
        
install(FILES libkyprocess.h
        DESTINATION include/lingmosdk/lingmosdk-system)
