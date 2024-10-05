aux_source_directory(. SOURCESCODE)
include(FindPkgConfig)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0 glib-2.0)
include_directories(${GTK3_INCLUDE_DIRS})

link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_library(kygsetting SHARED ${SOURCESCODE})
set_target_properties(kygsetting PROPERTIES VERSION 2.0.0 SOVERSION 1)

add_executable(test-kygsetting test/test-gsetting.c)
target_link_libraries(kygsetting ${GTK3_LIBRARIES})
target_link_libraries(test-kygsetting kygsetting)

install(TARGETS kygsetting
        DESTINATION lib/lingmosdk/lingmosdk-base)

install(FILES libkygsetting.h
        DESTINATION include/lingmosdk/lingmosdk-base)