#include "gui.hpp"
#include "utils.hpp"
#include <iostream>

bool SetupGLFW() 
{
    if(!glfwInit())
    {
        printf("Failed to initialize GLFW\n");
        glfwTerminate();
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return true;
}

void Cleanup(GLFWwindow* window) 
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void SetupImGuiStyleAndFont() 
{
    ImGuiIO& io = ImGui::GetIO();
    
    // Load a larger font (e.g., 20px)
    if(io.Fonts->AddFontFromFileTTF("../fonts/ITCAvantGardeStd-Bk.otf", 20.0f))
        ImGui_ImplOpenGL3_CreateFontsTexture();
}

// Callback for handling window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
}

void RenderCustom()
{
        // Make the ImGui window fill the entire GLFW window
        ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_height), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("##MainWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        // Fixed-position widgets inside the fullscreen window
        ImGui::SetCursorPos(ImVec2(50, 50));
        ImGui::Text("This window fills the entire screen!");

        ImGui::SetCursorPos(ImVec2(50, 100));
        static char buffer[128] = "Type here...";
        ImGui::InputText("##Input", buffer, IM_ARRAYSIZE(buffer));

        ImGui::SetCursorPos(ImVec2(50, 150));
        if (ImGui::Button("Click Me")) {
            // Button action
        }

        ImGui::End();
}


void RenderCustom2()
{
        // Center the UI
        ImGui::SetNextWindowSize(ImVec2(500, 250), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(50, 25), ImGuiCond_Always);
        ImGui::Begin("Copy File to /etc/systemd/", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::Text("Enter the file path:");
        ImGui::InputText("##SourceFile", sourceFile, IM_ARRAYSIZE(sourceFile));

        ImGui::Text("Enter sudo password:");
        ImGui::SameLine();
        if (ImGui::Button(showPassword ? "🙈 Hide" : "👁 Show")) {
            showPassword = !showPassword;
        }
        ImGui::InputText("##Password", password, IM_ARRAYSIZE(password), showPassword ? 0 : ImGuiInputTextFlags_Password);

        if (ImGui::Button("Copy File")) {
            std::string dest = "/etc/systemd/system/";
            dest += std::string(sourceFile).substr(std::string(sourceFile).find_last_of("/") + 1);

            if (copyFileWithSudo(sourceFile, dest, password)) {
                statusMessage = "✅ File copied successfully!";
            } else {
                statusMessage = "❌ Failed to copy file. Check path and permissions.";
            }
        }

        ImGui::Text("%s", statusMessage.c_str());  // Show status message
        ImGui::End();
}


void RenderCustom3()
{
        if (ImGui::Button("Modal"))
            ImGui::OpenPopup("Modal window");

        bool open = true;
        if (ImGui::BeginPopupModal("Modal window", &open))
        {
            ImGui::Text("Hello dsjfhds fhjs hfj dshfj hds");
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

}

void RenderCustom4(GLFWwindow * window)
{
    // Create main window with a menu bar
    ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar);

    // Menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
                std::cout << "Open selected\n";
            }
            if (ImGui::MenuItem("Save")) {
                std::cout << "Save selected\n";
            }
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                std::cout << "Undo selected\n";
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                std::cout << "Redo selected\n";
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Preferences")) {
                showAbout = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                showAbout = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("This is the main content area.");
    ImGui::End(); // End main window

    // About dialog
    if (showAbout) {
        ImGui::OpenPopup("About");
    }
    if (ImGui::BeginPopupModal("About", &showAbout, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("ImGui Menu Bar Example");
        if (ImGui::Button("Close")) {
            showAbout = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

bool StartGUI()
{
    if(!SetupGLFW())
    {
        printf("Failed to Setup GLFW\n");
        return false;
    }
    GLFWwindow * window = glfwCreateWindow(window_width,window_height, "ZettaListener", nullptr,nullptr);
    if(!window) 
    {
        printf("Failed to initialize window\n"); 
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL();
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SetupImGuiStyleAndFont();

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderCustom4(window);
        
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0,0,display_w,display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    Cleanup(window);
    return true;
    
}