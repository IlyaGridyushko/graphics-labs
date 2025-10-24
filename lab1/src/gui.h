#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/backends/imgui_impl_glfw.h"
#include "../third_party/imgui/backends/imgui_impl_opengl3.h"   

#include <string>
#include "Color.h"

class ColorConverterGUI {
public:
    ColorConverterGUI();
    ~ColorConverterGUI();
    
    void run();

private:
    // Window and OpenGL context
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    
    // cur color in different models
    Color color;
    
    
    bool initializeWindow();
    void initializeImGui();


    void renderGUI();
  
};