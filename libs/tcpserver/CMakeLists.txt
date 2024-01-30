add_library(tcpserver tcpserver.cpp tcpsession.cpp)

set(BOOST_ROOT_DIR "/home/navam//Downloads/boost_1_83_0")

set(MS_LOGGER_DIR ${CMAKE_SOURCE_DIR}/libs/mslogger)

target_include_directories(
    tcpserver PUBLIC 
    ${MS_LOGGER_DIR}
    ${BOOST_ROOT_DIR} 
)