cmake_minimum_required(VERSION 3.11)
project(ZettaListener LANGUAGES CXX C)
set (CMAKE_CXX_STANDARD 17)

set(TARGET_NAME ${PROJECT_NAME})

add_subdirectory(libs/tcpserver)
add_subdirectory(libs/xmlparser)
add_subdirectory(libs/dbcon)
add_subdirectory(libs/webapi)

add_executable(${TARGET_NAME} main.cpp)

target_include_directories(
    ${TARGET_NAME} PUBLIC 
    libs/tcpserver 
    libs/xmlparser 
    libs/dbcon 
    libs/mslogger
    libs/webapi
)

target_link_libraries(
    ${TARGET_NAME}
    tcpserver 
    xmlparser
    dbcon
    webapi
)


# how do i configure CMake to build tests from here and also build 
# those test-executables inside the same CMAKE_BUILD_DIRECTORY