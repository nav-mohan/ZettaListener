cmake_minimum_required(VERSION 3.11)
project(ZettaListener LANGUAGES CXX C)
set (CMAKE_CXX_STANDARD 17)

set(TARGET_NAME ${PROJECT_NAME})

add_subdirectory(libs/tcpserver)
add_subdirectory(libs/xmlparser)
add_subdirectory(libs/dbcon)
add_subdirectory(libs/webapi)

# must build GLAD and GLFW before IMGUI
# disable WAYLAND backend for GLFW because I dont have wayland dev-tools
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "build for wayland backend")
add_subdirectory(libs/glfw)
add_subdirectory(libs/glad)

# build-rules for IMGUI (ImGUI source-code does not have a CMakeLists)
set (IMGUI_DIR libs/imgui)
set (IMGUI_ESSENTIALS_SRC  
	${IMGUI_DIR}/imgui.cpp 
	${IMGUI_DIR}/imgui_draw.cpp 
	${IMGUI_DIR}/imgui_widgets.cpp 
	${IMGUI_DIR}/imgui_tables.cpp 
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp 
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        )
add_library(imgui ${IMGUI_ESSENTIALS_SRC})
target_link_libraries(imgui glfw)
target_include_directories(imgui PUBLIC ${IMGUI_DIR})


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
    glfw
    glad
    imgui
)


# how do i configure CMake to build tests from here and also build 
# those test-executables inside the same CMAKE_BUILD_DIRECTORY