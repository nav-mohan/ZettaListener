add_library(dbcon dbcon.cpp)

find_package(SQLite3 REQUIRED)

set(MS_LOGGER_DIR ${CMAKE_SOURCE_DIR}/libs/mslogger)

target_include_directories(
    dbcon PUBLIC 
    ${MS_LOGGER_DIR}
    ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(
    dbcon PUBLIC
    ${SQLite3_LIBRARIES}

)