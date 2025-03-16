#include "ms_logger.hpp"
#include "tcpserver.hpp"
#include "xmlparser.hpp"
#include "dbcon.hpp"
#include "webapi.hpp"
#include <functional>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char* glsl_version = "#version 330";
void SetupGLFW() 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Cleanup(GLFWwindow* window) 
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void SetupImGuiStyleAndFont() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Load a larger font (e.g., 20px)
    if(io.Fonts->AddFontFromFileTTF("../fonts/ITCAvantGardeStd-Bk.otf", 20.0f))
        ImGui_ImplOpenGL3_CreateFontsTexture();
}

int main(int argc, char *argv[])
{
    SetupGLFW();
    GLFWwindow * window = glfwCreateWindow(800,600, "ZettaListener", nullptr,nullptr);
    if(window == NULL) {printf("Failed to initialize window\n"); glfwTerminate();}
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SetupImGuiStyleAndFont();

    if (argc != 3)
    {
        printf("USAGE:   %s <TCP-PORT> <HTTP-PORT>\n",argv[0]);
        printf("EXAMPLE: %s 10001 8001\n",argv[0]);
        return 1;
    }
    int tcpPort = atoi(argv[1]);
    int httpPort = atoi(argv[2]);

    sqlite3 *db;
    int retval = sqlite3_open("zettalogger.db", &db);
    if (retval != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    DbCon dbc(db);

    boost::asio::io_context io_context;

    WebApi<DbCon> webapi(io_context, "0.0.0.0",httpPort, db); // i dont want the WebAPI making virtual calls
    
    auto writeToDatabase = [&dbc](std::unordered_map<std::string,std::string> record)
    {
        dbc.insertRecord(record);
        auto latestRecord = dbc.getLatest();
        // for(const auto [key,val] : latestRecord)
        // {
            // printf("LATEST %s --> %s\n",key.c_str(),val.c_str());
        // }
    };
    TCPServer<ZettaFullXmlParser> s(io_context, tcpPort, writeToDatabase);

    io_context.run();
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0,0,display_w,display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    Cleanup(window);

    return 0;
}