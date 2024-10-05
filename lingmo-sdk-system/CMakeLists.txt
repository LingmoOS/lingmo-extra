cmake_minimum_required(VERSION 3.1.3)

project(lingmosdk-system)

include(CMakePackageConfigHelpers)
include(FindPkgConfig)
pkg_check_modules(PKGS REQUIRED
                dbus-1
                glib-2.0
                dbus-glib-1
                libudev
                )
include_directories(${PKGS_INCLUDE_DIRS})
include_directories("${PROJECT_BINARY_DIR}")
include_directories("/usr/include/lingmosdk/lingmosdk-base")
link_directories("/usr/lib/lingmosdk/lingmosdk-base")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    add_definitions(-DRELEASE_BUILD)
    set(CMAKE_C_FLAGS "$ENV{CFLAGS} -O2 -Wall")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O2 -Wall")
else (CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    message("CMake Build Type: ${CMAKE_BUILD_TYPE}")
    set(CMAKE_C_FLAGS "$ENV{CFLAGS} -g -O0 -Wall")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -g -O0 -Wall")
endif()

message("Build Type: ${CMAKE_BUILD_TYPE}")

set (CMAKE_INSTALL_PREFIX /usr)
set (CMAKE_INSTALL_RPATH /usr/lib/lingmosdk/lingmosdk-system;/usr/lib/lingmosdk/lingmosdk-base)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)
