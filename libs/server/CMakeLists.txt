add_library(server server.cpp session.cpp)

set(BOOST_ROOT_DIR "/home/navam//Downloads/boost_1_83_0")

target_include_directories(
    server PUBLIC 
    ${BOOST_ROOT_DIR} 
)