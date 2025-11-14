#pragma once

#include "Image.h"
#include <vector>
#include <functional>

class ImageProcessor {
public:
    // Histogram operations
    static std::vector<int> calculateHistogram(const Image& img, int channel = 0);
    static Image equalizeHistogram(const Image& img);
    static Image equalizeHistogramRGB(const Image& img);
    static Image equalizeHistogramHSV(const Image& img);
    
    // Contrast enhancement
    static Image linearContrast(const Image& img, float alpha = 1.5f, float beta = 0.0f);
    static Image autoLinearContrast(const Image& img);
    
    // Filters - Low-pass (smoothing)
    static Image averageFilter(const Image& img, int kernelSize = 3);
    static Image gaussianFilter(const Image& img, int kernelSize = 3, float sigma = 1.0f);
    
    // Filters - High-pass (sharpening)
    static Image sharpenFilter(const Image& img);
    static Image laplacianFilter(const Image& img);
    static Image unsharpMasking(const Image& img, float amount = 1.5f);
    
    // Non-linear filters (order statistics)
    static Image medianFilter(const Image& img, int kernelSize = 3);
    static Image minFilter(const Image& img, int kernelSize = 3);
    static Image maxFilter(const Image& img, int kernelSize = 3);
    
    // Global thresholding
    static Image thresholdOtsu(const Image& img);
    static Image thresholdFixed(const Image& img, uint8_t threshold = 128);
    static Image thresholdMean(const Image& img);
    
    // Local thresholding
    static Image thresholdAdaptive(const Image& img, int blockSize = 15, float c = 5.0f);
    static Image thresholdNiblack(const Image& img, int windowSize = 15, float k = 0.2f);
    static Image thresholdSauvola(const Image& img, int windowSize = 15, float k = 0.5f);
    
    // Segmentation - Edge detection
    static Image sobelEdgeDetection(const Image& img);
    static Image prewittEdgeDetection(const Image& img);
    static Image cannyEdgeDetection(const Image& img, float lowThreshold = 50, float highThreshold = 150);
    
    // Morphological operations
    enum class MorphOp { ERODE, DILATE, OPEN, CLOSE };
    enum class StructElement { RECT, CROSS, ELLIPSE };
    static Image morphology(const Image& img, MorphOp op, int kernelSize = 3, StructElement element = StructElement::RECT);
    
private:
    // Helper functions
    static std::vector<std::vector<float>> createGaussianKernel(int size, float sigma);
    static std::vector<std::vector<int>> createStructuringElement(int size, StructElement element);
    static Image applyConvolution(const Image& img, const std::vector<std::vector<float>>& kernel);
    static uint8_t getMedian(std::vector<uint8_t>& values);
    static int calculateOtsuThreshold(const std::vector<int>& histogram);
    static float calculateLocalMean(const Image& img, int x, int y, int windowSize);
    static float calculateLocalStdDev(const Image& img, int x, int y, int windowSize, float mean);
    
    // RGB to HSV and back
    static void rgbToHsv(uint8_t r, uint8_t g, uint8_t b, float& h, float& s, float& v);
    static void hsvToRgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);
};
