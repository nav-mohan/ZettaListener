#ifndef GUI_HPP
#define GUI_HPP

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static const char* glsl_version = "#version 330";
static int window_width = 1280;
static int window_height = 720;
static bool showAbout = false;

bool SetupGLFW() ;
void Cleanup(GLFWwindow* window);
void SetupImGuiStyleAndFont();
// Callback for handling window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void RenderCustom();
void RenderCustom2();
void RenderCustom3();
void RenderCustom4(GLFWwindow * window);

bool StartGUI();

#endif //GUI_HPP