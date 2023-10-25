add_library(server server.cpp session.cpp)

set(BOOST_ROOT_DIR "/home/navam//Downloads/boost_1_83_0")

set(MS_LOGGER_DIR ${CMAKE_SOURCE_DIR}/libs/mslogger)

target_include_directories(
    server PUBLIC 
    ${MS_LOGGER_DIR}
    ${BOOST_ROOT_DIR} 
)