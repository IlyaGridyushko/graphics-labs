#include "gui.h"
#include <iostream>
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "render/thresholdControls.h"
#include "render/pointOperationsControls.h"
#include "render/imageDisplay.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

ImageProcessorGUI::ImageProcessorGUI() 
    : window(nullptr), windowWidth(1600), windowHeight(900), currentTab(0) {}

ImageProcessorGUI::~ImageProcessorGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool ImageProcessorGUI::initializeWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(windowWidth, windowHeight, "Lab 2 - Image Processing (Variant 6)", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return true;
}

void ImageProcessorGUI::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ItemSpacing = ImVec2(8, 6);
}

void ImageProcessorGUI::run() {
    if (!initializeWindow()) return;
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    initializeImGui();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        renderGUI();
        
        ImGui::Render();
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void ImageProcessorGUI::renderGUI() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, (float)windowHeight));
    
    ImGui::Begin("Image Processor - Variant 6", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    static char loadPath[512] = "";
    static char savePath[512] = "";

    ImGui::Text("Load Image:");
    ImGui::SameLine();
    ImGui::PushItemWidth(400);
    ImGui::InputText("##LoadPath", loadPath, sizeof(loadPath));
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.8f, 1.0f));
    if (ImGui::Button("Load", ImVec2(80, 0))) {
        if (loadPath[0] != '\0') {
            loadImage(std::string(loadPath));
        }
    }
    ImGui::PopStyleColor();

    ImGui::Text("Save Result:");
    ImGui::SameLine();
    ImGui::PushItemWidth(400);
    ImGui::InputText("##SavePath", savePath, sizeof(savePath));
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
    if (ImGui::Button("Save", ImVec2(80, 0))) {
        if (processedImage.getData() && savePath[0] != '\0') {
            std::string path = savePath;
            if (path.find(".png") == std::string::npos) {
                path += ".png";
            }
            saveImage(path);
        }
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));
    if (ImGui::Button("Reset", ImVec2(120, 30))) {
        if (originalImage.getData()) {
            processedImage = originalImage.clone();
        }
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    
    if (!currentImagePath.empty()) {
        ImGui::Text("File: %s", currentImagePath.c_str());
        if (originalImage.getData()) {
            ImGui::SameLine();
            ImGui::Text("| Size: %dx%d", originalImage.getWidth(), originalImage.getHeight());
        }
    } else {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No image loaded");
    }
    
    ImGui::Separator();

    if (ImGui::BeginTabBar("ProcessingTabs", ImGuiTabBarFlags_None)) {

        if (ImGui::BeginTabItem("Threshold Processing")) {
            ImGui::Columns(2, nullptr, true);
            ImGui::SetColumnWidth(0, 400);

            ImGui::BeginChild("ThresholdControls", ImVec2(0, -1), true);
            if (originalImage.getData()) {
                renderThresholdControls(originalImage, processedImage);
            } else {
                ImGui::TextWrapped("Load an image to start processing");
                ImGui::Spacing();
                ImGui::TextWrapped("This tab implements global thresholding methods for image binarization.");
            }
            ImGui::EndChild();

            ImGui::NextColumn();

            ImGui::BeginChild("ThresholdImages", ImVec2(0, -1), true);
            if (originalImage.getData()) {
                float imageWidth = ImGui::GetContentRegionAvail().x - 20;
                renderImageDisplay(originalImage, "Original Image", imageWidth);
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                if (processedImage.getData()) {
                    renderImageDisplay(processedImage, "Processed Image", imageWidth);
                }
            } else {
                ImGui::TextWrapped("No image loaded.\n\nClick 'Load Image' to get started.");
            }
            ImGui::EndChild();
            
            ImGui::Columns(1);
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Point Operations")) {
            ImGui::Columns(2, nullptr, true);
            ImGui::SetColumnWidth(0, 400);

            ImGui::BeginChild("PointControls", ImVec2(0, -1), true);
            if (originalImage.getData()) {
                renderPointOperationsControls(originalImage, processedImage);
            } else {
                ImGui::TextWrapped("Load an image to start processing");
                ImGui::Spacing();
                ImGui::TextWrapped("This tab implements point-wise operations and linear contrast enhancement.");
            }
            ImGui::EndChild();

            ImGui::NextColumn();

            ImGui::BeginChild("PointImages", ImVec2(0, -1), true);
            if (originalImage.getData()) {
                float imageWidth = ImGui::GetContentRegionAvail().x - 20;
                renderImageDisplay(originalImage, "Original Image", imageWidth);
                
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                if (processedImage.getData()) {
                    renderImageDisplay(processedImage, "Processed Image", imageWidth);
                }
            } else {
                ImGui::TextWrapped("No image loaded.\n\nClick 'Load Image' to get started.");
            }
            ImGui::EndChild();
            
            ImGui::Columns(1);
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("About")) {
            ImGui::TextWrapped("Laboratory Work 2 - Image Processing");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Text("Variant: 6");
            ImGui::Spacing();
            
            ImGui::TextWrapped("Implemented Methods:");
            ImGui::BulletText("Global Threshold Processing (Otsu, Triangle)");
            ImGui::BulletText("Point Operations (Brightness, Contrast, Gamma, etc.)");
            ImGui::BulletText("Linear Contrast Enhancement");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::TextWrapped("Instructions:");
            ImGui::BulletText("Load an image using the 'Load Image' button");
            ImGui::BulletText("Select a processing tab (Threshold or Point Operations)");
            ImGui::BulletText("Choose a method and adjust parameters");
            ImGui::BulletText("Click 'Apply' to process the image");
            ImGui::BulletText("Save the result using 'Save Result' button");
            ImGui::BulletText("Use 'Reset' to restore the original image");
            
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

void ImageProcessorGUI::loadImage(const std::string& filepath) {
    if (originalImage.load(filepath)) {
        processedImage = originalImage.clone();

        size_t lastSlash = filepath.find_last_of("/\\");
        currentImagePath = (lastSlash != std::string::npos) ?
                          filepath.substr(lastSlash + 1) : filepath;
    }
}

void ImageProcessorGUI::saveImage(const std::string& filepath) {
    if (processedImage.save(filepath)) {
        std::cout << "Image saved successfully: " << filepath << std::endl;
    } else {
        std::cerr << "Failed to save image: " << filepath << std::endl;
    }
}