#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Image.h"
#include <string>

class ImageProcessorGUI {
public:
    ImageProcessorGUI();
    ~ImageProcessorGUI();
    
    bool initializeWindow();
    void initializeImGui();
    void run();

private:
    void renderGUI();
    void loadImage(const std::string& filepath);
    void saveImage(const std::string& filepath);
    
    GLFWwindow* window;
    int windowWidth, windowHeight;
    
    Image originalImage;
    Image processedImage;
    
    int currentTab;
    std::string currentImagePath;
};