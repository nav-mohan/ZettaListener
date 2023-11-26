cmake_minimum_required(VERSION 3.11)
project(ZettaListener LANGUAGES CXX C)
set (CMAKE_CXX_STANDARD 17)

set(TARGET_NAME ${PROJECT_NAME})

add_subdirectory(libs/server)
add_subdirectory(libs/xmlparser)
add_executable(${TARGET_NAME} main.cpp)

target_include_directories(
    ${TARGET_NAME} PUBLIC 
    libs/server 
    libs/xmlparser 
    libs/mslogger
)

target_link_libraries(
    ${TARGET_NAME}
    server 
    xmlparser
)