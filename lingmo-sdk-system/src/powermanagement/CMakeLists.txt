cmake_minimum_required(VERSION 3.5)

project(kypowermanagement LANGUAGES CXX)

set(POWERMANAGEMENT_TOP_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(${PROJECT_NAME} SHARED)
set_target_properties(kypowermanagement PROPERTIES VERSION 2.0.0 SOVERSION 1)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -g)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS-1 dbus-1)
target_include_directories(${PROJECT_NAME} PRIVATE ${DBUS-1_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${DBUS-1_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${DBUS-1_LIBRARIES})

pkg_check_modules(KYLOG lingmosdk-log)
target_include_directories(${PROJECT_NAME} PRIVATE ${KYLOG_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${KYLOG_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${KYLOG_LIBRARIES})

set(SRCS 
    "${POWERMANAGEMENT_TOP_DIR}/lockscreen.cpp"
    "${POWERMANAGEMENT_TOP_DIR}/libkylockscreen.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE ${POWERMANAGEMENT_TOP_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${SRCS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib/lingmosdk/lingmosdk-system)