aux_source_directory(. SOURCESCODE)
add_library(kyutils SHARED ${SOURCESCODE})
set_target_properties(kyutils PROPERTIES VERSION 2.0.0 SOVERSION 1)
include_directories(.)
install(TARGETS kyutils
        DESTINATION lib/lingmosdk/lingmosdk-base)
# install(FILES sdkmarcos.h kerr.h cstring-extension.h
#         DESTINATION include/lingmosdk/lingmosdk-base)
add_executable(test-utils sample/kyutils_sample.c)
target_link_libraries(test-utils kyutils)

add_subdirectory(data-structure)