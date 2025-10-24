
#include "gui.h"
#include <iostream>
#include <sstream>
#include <iomanip>

#include "render/hsvControls.h"
#include "render/cmykControls.h"
#include "render/rgbControls.h"
#include "render/extraControls.h"
#include "../utils/SystemInfo.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

ColorConverterGUI::ColorConverterGUI() 
    : window(nullptr), windowWidth(800), windowHeight(800) {
    color.setRGB(1.0f, 0.0f, 0.0f);
}

ColorConverterGUI::~ColorConverterGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool ColorConverterGUI::initializeWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(windowWidth, windowHeight, "Color Converter", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    return true;
}

void ColorConverterGUI::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ItemSpacing = ImVec2(8, 6);

    //font
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("../PakenhamBl Italic.ttf", 20.0f);
    io.ConfigDebugHighlightIdConflicts = false;
}

void ColorConverterGUI::run() {
    if (!initializeWindow()) {
        return;
    }
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    initializeImGui();
    //SystemInfo::printAll();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        renderGUI();
        
        ImGui::Render();
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void ColorConverterGUI::renderGUI() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    
    ImGui::Begin("Color Model Converter", nullptr, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    //labels
    ImGui::Columns(3, "color_models", true);
    ImGui::Text("RGB Color Model");
    ImGui::NextColumn();
    ImGui::Text("CMYK Color Model");
    ImGui::NextColumn();
    ImGui::Text("HSV Color Model");
    ImGui::Columns(1);

    ImGui::Columns(3, "color_models", true);
    // Column 1 RGB
    renderRGBControls(this->color);
    ImGui::NextColumn();
    // Column 2 CMYK
    renderCMYKControls(this->color);
    ImGui::NextColumn();
    // Column 3 HSV
    renderHSVControls(this->color);
    ImGui::Columns(1);
    
    renderExtraControls(this->color,this->windowWidth);
}

