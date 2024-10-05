cmake_minimum_required(VERSION 3.1.3)

project(lingmosdk-base)

include(CMakePackageConfigHelpers)

include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-Drelease)
    set(CMAKE_C_FLAGS "$ENV{CFLAGS} -O1 -Wall")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O1 -Wall")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_C_FLAGS "$ENV{CFLAGS} -g -O0 -Wall")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -g -O0 -Wall")
endif()

message("Build Type: ${CMAKE_BUILD_TYPE}")

set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set (CMAKE_INSTALL_PREFIX /usr)
set (CMAKE_INSTALL_RPATH /usr/lib/lingmosdk/lingmosdk-base)

add_subdirectory(src)