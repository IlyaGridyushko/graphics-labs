#include "gui.h"
#include <iostream>
#include <cmath>

// For file dialogs - simplified version using console input
#include <fstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

ImageProcessingGUI::ImageProcessingGUI() 
    : window(nullptr), windowWidth(1600), windowHeight(900),
      originalTexture(0), processedTexture(0),
      selectedOperation(0), contrastAlpha(1.5f), contrastBeta(0.0f),
      filterKernelSize(3), gaussianSigma(1.0f), unsharpAmount(1.5f),
      fixedThreshold(128), adaptiveBlockSize(15), adaptiveC(5.0f),
      niblackWindowSize(15), niblackK(0.2f),
      sauvolaWindowSize(15), sauvolaK(0.5f),
      cannyLowThreshold(50.0f), cannyHighThreshold(150.0f),
      morphKernelSize(3), morphOperation(0), morphElement(0),
      showHistogram(false), histogramChannel(0) {
}

ImageProcessingGUI::~ImageProcessingGUI() {
    if (originalTexture) glDeleteTextures(1, &originalTexture);
    if (processedTexture) glDeleteTextures(1, &processedTexture);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool ImageProcessingGUI::initializeWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(windowWidth, windowHeight, "Lab 2 - Image Processing", NULL, NULL);
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

void ImageProcessingGUI::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ItemSpacing = ImVec2(8, 6);
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigDebugHighlightIdConflicts = false;
}

void ImageProcessingGUI::run() {
    if (!initializeWindow()) {
        return;
    }
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    initializeImGui();
    
    // Try to load a default test image
    std::string defaultImage = "../test_images/test.png";
    if (originalImage.load(defaultImage)) {
        createTextureFromImage(originalImage, originalTexture);
        processedImage = originalImage.clone();
        createTextureFromImage(processedImage, processedTexture);
    }

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

void ImageProcessingGUI::renderGUI() {
    // Main control panel
    renderControlsWindow();
    
    // Display original and processed images
    if (originalImage.isValid()) {
        renderImageWindow("Original Image", originalTexture, 
                         originalImage.getWidth(), originalImage.getHeight(), 
                         400, 0);
    }
    
    if (processedImage.isValid()) {
        renderImageWindow("Processed Image", processedTexture,
                         processedImage.getWidth(), processedImage.getHeight(),
                         400, originalImage.isValid() ? 450 : 0);
    }
    
    // Histogram window
    if (showHistogram && processedImage.isValid()) {
        renderHistogramWindow();
    }
}

void ImageProcessingGUI::renderControlsWindow() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(390, static_cast<float>(windowHeight)), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Image Processing Controls", nullptr, ImGuiWindowFlags_NoResize);
    
    ImGui::Text("Lab 2: Digital Image Processing");
    ImGui::Separator();
    
    if (ImGui::Button("Load Image", ImVec2(180, 30))) {
        loadImage();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Image", ImVec2(180, 30))) {
        saveImage();
    }
    
    ImGui::Separator();
    ImGui::Text("Select Operation:");
    
    const char* operations[] = {
        "None",
        "Linear Contrast",
        "Auto Linear Contrast",
        "Histogram Equalization (Gray)",
        "Histogram Equalization (RGB)",
        "Histogram Equalization (HSV)",
        "Average Filter",
        "Gaussian Filter",
        "Sharpen Filter",
        "Laplacian Filter",
        "Unsharp Masking",
        "Median Filter",
        "Min Filter",
        "Max Filter",
        "Threshold - Otsu",
        "Threshold - Fixed",
        "Threshold - Mean",
        "Threshold - Adaptive",
        "Threshold - Niblack",
        "Threshold - Sauvola",
        "Sobel Edge Detection",
        "Prewitt Edge Detection",
        "Canny Edge Detection",
        "Morphology - Erosion",
        "Morphology - Dilation",
        "Morphology - Opening",
        "Morphology - Closing"
    };
    
    if (ImGui::Combo("##Operation", &selectedOperation, operations, IM_ARRAYSIZE(operations))) {
        applyOperation();
    }
    
    ImGui::Separator();
    ImGui::Text("Parameters:");
    
    bool paramsChanged = false;
    
    // Linear Contrast parameters
    if (selectedOperation == 1) {
        paramsChanged |= ImGui::SliderFloat("Alpha (contrast)", &contrastAlpha, 0.1f, 3.0f);
        paramsChanged |= ImGui::SliderFloat("Beta (brightness)", &contrastBeta, -100.0f, 100.0f);
    }
    
    // Filter kernel size
    if (selectedOperation >= 6 && selectedOperation <= 13) {
        paramsChanged |= ImGui::SliderInt("Kernel Size", &filterKernelSize, 3, 15);
        if (filterKernelSize % 2 == 0) filterKernelSize++;
    }
    
    // Gaussian sigma
    if (selectedOperation == 7) {
        paramsChanged |= ImGui::SliderFloat("Sigma", &gaussianSigma, 0.1f, 5.0f);
    }
    
    // Unsharp masking
    if (selectedOperation == 10) {
        paramsChanged |= ImGui::SliderFloat("Amount", &unsharpAmount, 0.5f, 3.0f);
    }
    
    // Fixed threshold
    if (selectedOperation == 15) {
        paramsChanged |= ImGui::SliderInt("Threshold", reinterpret_cast<int*>(&fixedThreshold), 0, 255);
    }
    
    // Adaptive threshold
    if (selectedOperation == 17) {
        paramsChanged |= ImGui::SliderInt("Block Size", &adaptiveBlockSize, 3, 51);
        if (adaptiveBlockSize % 2 == 0) adaptiveBlockSize++;
        paramsChanged |= ImGui::SliderFloat("C", &adaptiveC, -50.0f, 50.0f);
    }
    
    // Niblack threshold
    if (selectedOperation == 18) {
        paramsChanged |= ImGui::SliderInt("Window Size", &niblackWindowSize, 3, 51);
        if (niblackWindowSize % 2 == 0) niblackWindowSize++;
        paramsChanged |= ImGui::SliderFloat("k", &niblackK, -1.0f, 1.0f);
    }
    
    // Sauvola threshold
    if (selectedOperation == 19) {
        paramsChanged |= ImGui::SliderInt("Window Size", &sauvolaWindowSize, 3, 51);
        if (sauvolaWindowSize % 2 == 0) sauvolaWindowSize++;
        paramsChanged |= ImGui::SliderFloat("k", &sauvolaK, 0.0f, 1.0f);
    }
    
    // Canny edge detection
    if (selectedOperation == 22) {
        paramsChanged |= ImGui::SliderFloat("Low Threshold", &cannyLowThreshold, 0.0f, 255.0f);
        paramsChanged |= ImGui::SliderFloat("High Threshold", &cannyHighThreshold, 0.0f, 255.0f);
    }
    
    // Morphological operations
    if (selectedOperation >= 23 && selectedOperation <= 26) {
        paramsChanged |= ImGui::SliderInt("Kernel Size", &morphKernelSize, 3, 15);
        if (morphKernelSize % 2 == 0) morphKernelSize++;
        
        const char* elements[] = {"Rectangle", "Cross", "Ellipse"};
        paramsChanged |= ImGui::Combo("Element", &morphElement, elements, 3);
    }
    
    if (paramsChanged) {
        applyOperation();
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Apply Operation", ImVec2(370, 35))) {
        applyOperation();
    }
    
    ImGui::Separator();
    ImGui::Checkbox("Show Histogram", &showHistogram);
    
    if (showHistogram && processedImage.isValid()) {
        if (processedImage.getChannels() > 1) {
            const char* channels[] = {"Red", "Green", "Blue"};
            ImGui::Combo("Channel", &histogramChannel, channels, 3);
        }
    }
    
    ImGui::Separator();
    if (originalImage.isValid()) {
        ImGui::Text("Original: %dx%d, %d channels", 
                   originalImage.getWidth(), 
                   originalImage.getHeight(),
                   originalImage.getChannels());
    }
    if (processedImage.isValid()) {
        ImGui::Text("Processed: %dx%d, %d channels", 
                   processedImage.getWidth(), 
                   processedImage.getHeight(),
                   processedImage.getChannels());
    }
    
    ImGui::End();
}

void ImageProcessingGUI::renderImageWindow(const char* title, GLuint texture, 
                                          int width, int height, float posX, float posY) {
    ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 450), ImGuiCond_FirstUseEver);
    
    ImGui::Begin(title);
    
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    float aspectRatio = static_cast<float>(width) / height;
    
    float displayWidth = windowSize.x;
    float displayHeight = displayWidth / aspectRatio;
    
    if (displayHeight > windowSize.y) {
        displayHeight = windowSize.y;
        displayWidth = displayHeight * aspectRatio;
    }
    
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(texture)), 
                 ImVec2(displayWidth, displayHeight),
                 ImVec2(0, 0), ImVec2(1, 1));
    
    ImGui::End();
}

void ImageProcessingGUI::renderHistogramWindow() {
    ImGui::SetNextWindowPos(ImVec2(1000, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Histogram");
    
    int channel = processedImage.getChannels() == 1 ? 0 : histogramChannel;
    currentHistogram = ImageProcessor::calculateHistogram(processedImage, channel);
    
    // Find max value for scaling
    int maxCount = *std::max_element(currentHistogram.begin(), currentHistogram.end());
    
    // Convert to float for plotting
    std::vector<float> histogramFloat(256);
    for (int i = 0; i < 256; i++) {
        histogramFloat[i] = static_cast<float>(currentHistogram[i]);
    }
    
    ImGui::PlotHistogram("##Histogram", histogramFloat.data(), 256, 0, nullptr, 
                        0.0f, static_cast<float>(maxCount), ImVec2(580, 200));
    
    ImGui::Text("Max count: %d", maxCount);
    
    ImGui::End();
}

void ImageProcessingGUI::loadImage() {
    std::cout << "Enter image path to load: ";
    std::string path;
    std::getline(std::cin, path);
    
    if (originalImage.load(path)) {
        createTextureFromImage(originalImage, originalTexture);
        processedImage = originalImage.clone();
        createTextureFromImage(processedImage, processedTexture);
        std::cout << "Image loaded successfully!" << std::endl;
    } else {
        std::cerr << "Failed to load image: " << path << std::endl;
    }
}

void ImageProcessingGUI::saveImage() {
    if (!processedImage.isValid()) {
        std::cerr << "No processed image to save!" << std::endl;
        return;
    }
    
    std::cout << "Enter path to save image: ";
    std::string path;
    std::getline(std::cin, path);
    
    if (processedImage.save(path)) {
        std::cout << "Image saved successfully!" << std::endl;
    } else {
        std::cerr << "Failed to save image: " << path << std::endl;
    }
}

void ImageProcessingGUI::applyOperation() {
    if (!originalImage.isValid()) {
        std::cerr << "No image loaded!" << std::endl;
        return;
    }
    
    std::cout << "Applying operation " << selectedOperation << "..." << std::endl;
    
    switch (selectedOperation) {
        case 0: // None
            processedImage = originalImage.clone();
            break;
        case 1: // Linear Contrast
            processedImage = ImageProcessor::linearContrast(originalImage, contrastAlpha, contrastBeta);
            break;
        case 2: // Auto Linear Contrast
            processedImage = ImageProcessor::autoLinearContrast(originalImage);
            break;
        case 3: // Histogram Equalization (Gray)
            processedImage = ImageProcessor::equalizeHistogram(originalImage);
            break;
        case 4: // Histogram Equalization (RGB)
            processedImage = ImageProcessor::equalizeHistogramRGB(originalImage);
            break;
        case 5: // Histogram Equalization (HSV)
            processedImage = ImageProcessor::equalizeHistogramHSV(originalImage);
            break;
        case 6: // Average Filter
            processedImage = ImageProcessor::averageFilter(originalImage, filterKernelSize);
            break;
        case 7: // Gaussian Filter
            processedImage = ImageProcessor::gaussianFilter(originalImage, filterKernelSize, gaussianSigma);
            break;
        case 8: // Sharpen Filter
            processedImage = ImageProcessor::sharpenFilter(originalImage);
            break;
        case 9: // Laplacian Filter
            processedImage = ImageProcessor::laplacianFilter(originalImage);
            break;
        case 10: // Unsharp Masking
            processedImage = ImageProcessor::unsharpMasking(originalImage, unsharpAmount);
            break;
        case 11: // Median Filter
            processedImage = ImageProcessor::medianFilter(originalImage, filterKernelSize);
            break;
        case 12: // Min Filter
            processedImage = ImageProcessor::minFilter(originalImage, filterKernelSize);
            break;
        case 13: // Max Filter
            processedImage = ImageProcessor::maxFilter(originalImage, filterKernelSize);
            break;
        case 14: // Threshold - Otsu
            processedImage = ImageProcessor::thresholdOtsu(originalImage);
            break;
        case 15: // Threshold - Fixed
            processedImage = ImageProcessor::thresholdFixed(originalImage, fixedThreshold);
            break;
        case 16: // Threshold - Mean
            processedImage = ImageProcessor::thresholdMean(originalImage);
            break;
        case 17: // Threshold - Adaptive
            processedImage = ImageProcessor::thresholdAdaptive(originalImage, adaptiveBlockSize, adaptiveC);
            break;
        case 18: // Threshold - Niblack
            processedImage = ImageProcessor::thresholdNiblack(originalImage, niblackWindowSize, niblackK);
            break;
        case 19: // Threshold - Sauvola
            processedImage = ImageProcessor::thresholdSauvola(originalImage, sauvolaWindowSize, sauvolaK);
            break;
        case 20: // Sobel Edge Detection
            processedImage = ImageProcessor::sobelEdgeDetection(originalImage);
            break;
        case 21: // Prewitt Edge Detection
            processedImage = ImageProcessor::prewittEdgeDetection(originalImage);
            break;
        case 22: // Canny Edge Detection
            processedImage = ImageProcessor::cannyEdgeDetection(originalImage, cannyLowThreshold, cannyHighThreshold);
            break;
        case 23: // Morphology - Erosion
            processedImage = ImageProcessor::morphology(originalImage, ImageProcessor::MorphOp::ERODE, 
                                                       morphKernelSize, static_cast<ImageProcessor::StructElement>(morphElement));
            break;
        case 24: // Morphology - Dilation
            processedImage = ImageProcessor::morphology(originalImage, ImageProcessor::MorphOp::DILATE,
                                                       morphKernelSize, static_cast<ImageProcessor::StructElement>(morphElement));
            break;
        case 25: // Morphology - Opening
            processedImage = ImageProcessor::morphology(originalImage, ImageProcessor::MorphOp::OPEN,
                                                       morphKernelSize, static_cast<ImageProcessor::StructElement>(morphElement));
            break;
        case 26: // Morphology - Closing
            processedImage = ImageProcessor::morphology(originalImage, ImageProcessor::MorphOp::CLOSE,
                                                       morphKernelSize, static_cast<ImageProcessor::StructElement>(morphElement));
            break;
    }
    
    updateProcessedTexture();
    std::cout << "Operation completed!" << std::endl;
}

void ImageProcessingGUI::createTextureFromImage(const Image& img, GLuint& texture) {
    if (texture) {
        glDeleteTextures(1, &texture);
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLenum format = GL_RGB;
    if (img.getChannels() == 1) format = GL_RED;
    else if (img.getChannels() == 3) format = GL_RGB;
    else if (img.getChannels() == 4) format = GL_RGBA;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, img.getWidth(), img.getHeight(), 
                 0, format, GL_UNSIGNED_BYTE, img.getData());
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ImageProcessingGUI::updateProcessedTexture() {
    createTextureFromImage(processedImage, processedTexture);
}
