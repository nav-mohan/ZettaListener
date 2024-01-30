add_library(webapi webapi.cpp httplistener.cpp httpsession.cpp)

set(BOOST_ROOT_DIR "/home/navam//Downloads/boost_1_83_0")

set(MS_LOGGER_DIR ${CMAKE_SOURCE_DIR}/libs/mslogger)

target_include_directories(
    webapi PUBLIC 
    ${MS_LOGGER_DIR}
    ${BOOST_ROOT_DIR} 
)