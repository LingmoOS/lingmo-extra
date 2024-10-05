find_library(GLIBC_LIB glib-2.0)
include_directories("/usr/include/glib-2.0")
add_library(kylocation SHARED libkylocation.c)
set_target_properties(kylocation PROPERTIES VERSION 2.0.0 SOVERSION 1)
target_link_libraries(kylocation -lglib-2.0 -lgio-2.0)
add_executable(kylocation-test test/kylocation-test.c)
target_link_libraries(kylocation-test kylocation)

install(TARGETS kylocation
        DESTINATION lib/lingmosdk/lingmosdk-system)

install(FILES libkylocation.h
        DESTINATION include/lingmosdk/lingmosdk-system)
