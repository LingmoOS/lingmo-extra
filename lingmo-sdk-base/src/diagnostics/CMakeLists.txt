cmake_minimum_required(VERSION 3.5)

project(kydiagnostics LANGUAGES CXX)

set(DIAGNOSTICS_TOP_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(${PROJECT_NAME} SHARED)
set_target_properties(kydiagnostics PROPERTIES VERSION 2.0.0 SOVERSION 1)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

find_package(PkgConfig REQUIRED)

pkg_check_modules(OPENSSL openssl)
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_LIBRARIES})

pkg_check_modules(DBUS-1 dbus-1)
target_include_directories(${PROJECT_NAME} PRIVATE ${DBUS-1_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${DBUS-1_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${DBUS-1_LIBRARIES})


set(SRCS 
    "${DIAGNOSTICS_TOP_DIR}/libkydiagnostics.cpp"
    "${DIAGNOSTICS_TOP_DIR}/buriedpoint.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE ${DIAGNOSTICS_TOP_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${SRCS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib/lingmosdk/lingmosdk-base)