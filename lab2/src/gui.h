#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/backends/imgui_impl_glfw.h"
#include "../third_party/imgui/backends/imgui_impl_opengl3.h"

#include <string>
#include "Image.h"
#include "ImageProcessor.h"

class ImageProcessingGUI {
public:
    ImageProcessingGUI();
    ~ImageProcessingGUI();
    
    void run();

private:
    // Window and OpenGL context
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    
    // Images
    Image originalImage;
    Image processedImage;
    
    // OpenGL textures for displaying images
    GLuint originalTexture;
    GLuint processedTexture;
    
    // UI state
    int selectedOperation;
    float contrastAlpha;
    float contrastBeta;
    int filterKernelSize;
    float gaussianSigma;
    float unsharpAmount;
    uint8_t fixedThreshold;
    int adaptiveBlockSize;
    float adaptiveC;
    int niblackWindowSize;
    float niblackK;
    int sauvolaWindowSize;
    float sauvolaK;
    float cannyLowThreshold;
    float cannyHighThreshold;
    int morphKernelSize;
    int morphOperation;
    int morphElement;
    bool showHistogram;
    int histogramChannel;
    
    bool initializeWindow();
    void initializeImGui();
    void renderGUI();
    void renderImageWindow(const char* title, GLuint texture, int width, int height, float posX, float posY);
    void renderControlsWindow();
    void renderHistogramWindow();
    
    void loadImage();
    void saveImage();
    void applyOperation();
    void createTextureFromImage(const Image& img, GLuint& texture);
    void updateProcessedTexture();
    
    std::vector<int> currentHistogram;
};
