#include "ImageProcessor.h"
#include <algorithm>
#include <cmath>

// This file contains the continuation of ImageProcessor implementation

// ========== Global Thresholding ==========

int ImageProcessor::calculateOtsuThreshold(const std::vector<int>& histogram) {
    int total = 0;
    for (int i = 0; i < 256; i++) {
        total += histogram[i];
    }
    
    float sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += i * histogram[i];
    }
    
    float sumB = 0;
    int wB = 0;
    int wF = 0;
    
    float varMax = 0;
    int threshold = 0;
    
    for (int t = 0; t < 256; t++) {
        wB += histogram[t];
        if (wB == 0) continue;
        
        wF = total - wB;
        if (wF == 0) break;
        
        sumB += static_cast<float>(t * histogram[t]);
        
        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;
        
        float varBetween = static_cast<float>(wB) * static_cast<float>(wF) * (mB - mF) * (mB - mF);
        
        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = t;
        }
    }
    
    return threshold;
}

Image ImageProcessor::thresholdOtsu(const Image& img) {
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    std::vector<int> histogram = calculateHistogram(gray, 0);
    int threshold = calculateOtsuThreshold(histogram);
    
    return thresholdFixed(gray, threshold);
}

Image ImageProcessor::thresholdFixed(const Image& img, uint8_t threshold) {
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    Image result = gray.clone();
    
    for (int y = 0; y < result.getHeight(); y++) {
        for (int x = 0; x < result.getWidth(); x++) {
            uint8_t value = gray.getPixel(x, y, 0);
            result.setPixel(x, y, 0, value >= threshold ? 255 : 0);
        }
    }
    
    return result;
}

Image ImageProcessor::thresholdMean(const Image& img) {
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    
    long long sum = 0;
    for (int y = 0; y < gray.getHeight(); y++) {
        for (int x = 0; x < gray.getWidth(); x++) {
            sum += gray.getPixel(x, y, 0);
        }
    }
    
    uint8_t meanThreshold = static_cast<uint8_t>(sum / (gray.getWidth() * gray.getHeight()));
    return thresholdFixed(gray, meanThreshold);
}

// ========== Local Thresholding ==========

float ImageProcessor::calculateLocalMean(const Image& img, int x, int y, int windowSize) {
    int offset = windowSize / 2;
    float sum = 0.0f;
    int count = 0;
    
    for (int wy = -offset; wy <= offset; wy++) {
        for (int wx = -offset; wx <= offset; wx++) {
            int px = std::max(0, std::min(img.getWidth() - 1, x + wx));
            int py = std::max(0, std::min(img.getHeight() - 1, y + wy));
            sum += img.getPixel(px, py, 0);
            count++;
        }
    }
    
    return sum / count;
}

float ImageProcessor::calculateLocalStdDev(const Image& img, int x, int y, int windowSize, float mean) {
    int offset = windowSize / 2;
    float sumSq = 0.0f;
    int count = 0;
    
    for (int wy = -offset; wy <= offset; wy++) {
        for (int wx = -offset; wx <= offset; wx++) {
            int px = std::max(0, std::min(img.getWidth() - 1, x + wx));
            int py = std::max(0, std::min(img.getHeight() - 1, y + wy));
            float diff = img.getPixel(px, py, 0) - mean;
            sumSq += diff * diff;
            count++;
        }
    }
    
    return std::sqrt(sumSq / count);
}

Image ImageProcessor::thresholdAdaptive(const Image& img, int blockSize, float c) {
    if (blockSize % 2 == 0) blockSize++;
    
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    Image result(gray.getWidth(), gray.getHeight(), 1);
    
    for (int y = 0; y < gray.getHeight(); y++) {
        for (int x = 0; x < gray.getWidth(); x++) {
            float localMean = calculateLocalMean(gray, x, y, blockSize);
            uint8_t value = gray.getPixel(x, y, 0);
            uint8_t threshold = static_cast<uint8_t>(std::max(0.0f, localMean - c));
            result.setPixel(x, y, 0, value >= threshold ? 255 : 0);
        }
    }
    
    return result;
}

Image ImageProcessor::thresholdNiblack(const Image& img, int windowSize, float k) {
    if (windowSize % 2 == 0) windowSize++;
    
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    Image result(gray.getWidth(), gray.getHeight(), 1);
    
    for (int y = 0; y < gray.getHeight(); y++) {
        for (int x = 0; x < gray.getWidth(); x++) {
            float localMean = calculateLocalMean(gray, x, y, windowSize);
            float localStdDev = calculateLocalStdDev(gray, x, y, windowSize, localMean);
            
            float threshold = localMean + k * localStdDev;
            uint8_t value = gray.getPixel(x, y, 0);
            result.setPixel(x, y, 0, value >= threshold ? 255 : 0);
        }
    }
    
    return result;
}

Image ImageProcessor::thresholdSauvola(const Image& img, int windowSize, float k) {
    if (windowSize % 2 == 0) windowSize++;
    
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    Image result(gray.getWidth(), gray.getHeight(), 1);
    const float R = 128.0f;  // Dynamic range of standard deviation
    
    for (int y = 0; y < gray.getHeight(); y++) {
        for (int x = 0; x < gray.getWidth(); x++) {
            float localMean = calculateLocalMean(gray, x, y, windowSize);
            float localStdDev = calculateLocalStdDev(gray, x, y, windowSize, localMean);
            
            float threshold = localMean * (1.0f + k * ((localStdDev / R) - 1.0f));
            uint8_t value = gray.getPixel(x, y, 0);
            result.setPixel(x, y, 0, value >= threshold ? 255 : 0);
        }
    }
    
    return result;
}

// ========== Edge Detection ==========

Image ImageProcessor::sobelEdgeDetection(const Image& img) {
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    Image result(gray.getWidth(), gray.getHeight(), 1);
    
    std::vector<std::vector<float>> sobelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    
    std::vector<std::vector<float>> sobelY = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };
    
    for (int y = 1; y < gray.getHeight() - 1; y++) {
        for (int x = 1; x < gray.getWidth() - 1; x++) {
            float gx = 0.0f, gy = 0.0f;
            
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    uint8_t pixel = gray.getPixel(x + kx, y + ky, 0);
                    gx += pixel * sobelX[ky + 1][kx + 1];
                    gy += pixel * sobelY[ky + 1][kx + 1];
                }
            }
            
            float magnitude = std::sqrt(gx * gx + gy * gy);
            magnitude = std::min(255.0f, magnitude);
            result.setPixel(x, y, 0, static_cast<uint8_t>(magnitude));
        }
    }
    
    return result;
}

Image ImageProcessor::prewittEdgeDetection(const Image& img) {
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    Image result(gray.getWidth(), gray.getHeight(), 1);
    
    std::vector<std::vector<float>> prewittX = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    
    std::vector<std::vector<float>> prewittY = {
        {-1, -1, -1},
        { 0,  0,  0},
        { 1,  1,  1}
    };
    
    for (int y = 1; y < gray.getHeight() - 1; y++) {
        for (int x = 1; x < gray.getWidth() - 1; x++) {
            float gx = 0.0f, gy = 0.0f;
            
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    uint8_t pixel = gray.getPixel(x + kx, y + ky, 0);
                    gx += pixel * prewittX[ky + 1][kx + 1];
                    gy += pixel * prewittY[ky + 1][kx + 1];
                }
            }
            
            float magnitude = std::sqrt(gx * gx + gy * gy);
            magnitude = std::min(255.0f, magnitude);
            result.setPixel(x, y, 0, static_cast<uint8_t>(magnitude));
        }
    }
    
    return result;
}

Image ImageProcessor::cannyEdgeDetection(const Image& img, float lowThreshold, float highThreshold) {
    // Simplified Canny implementation
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    
    // 1. Apply Gaussian blur
    Image blurred = gaussianFilter(gray, 5, 1.4f);
    
    // 2. Apply Sobel to get gradient magnitude and direction
    Image result(blurred.getWidth(), blurred.getHeight(), 1);
    std::vector<std::vector<float>> gradientMag(blurred.getHeight(), std::vector<float>(blurred.getWidth()));
    
    for (int y = 1; y < blurred.getHeight() - 1; y++) {
        for (int x = 1; x < blurred.getWidth() - 1; x++) {
            float gx = -blurred.getPixel(x-1, y-1, 0) + blurred.getPixel(x+1, y-1, 0)
                      -2*blurred.getPixel(x-1, y, 0) + 2*blurred.getPixel(x+1, y, 0)
                      -blurred.getPixel(x-1, y+1, 0) + blurred.getPixel(x+1, y+1, 0);
            
            float gy = -blurred.getPixel(x-1, y-1, 0) - 2*blurred.getPixel(x, y-1, 0) - blurred.getPixel(x+1, y-1, 0)
                      +blurred.getPixel(x-1, y+1, 0) + 2*blurred.getPixel(x, y+1, 0) + blurred.getPixel(x+1, y+1, 0);
            
            gradientMag[y][x] = std::sqrt(gx * gx + gy * gy);
        }
    }
    
    // 3. Non-maximum suppression and thresholding (simplified)
    for (int y = 1; y < blurred.getHeight() - 1; y++) {
        for (int x = 1; x < blurred.getWidth() - 1; x++) {
            float mag = gradientMag[y][x];
            
            if (mag >= highThreshold) {
                result.setPixel(x, y, 0, 255);
            } else if (mag >= lowThreshold) {
                // Check if connected to strong edge
                bool connected = false;
                for (int dy = -1; dy <= 1 && !connected; dy++) {
                    for (int dx = -1; dx <= 1 && !connected; dx++) {
                        if (gradientMag[y+dy][x+dx] >= highThreshold) {
                            connected = true;
                        }
                    }
                }
                result.setPixel(x, y, 0, connected ? 255 : 0);
            } else {
                result.setPixel(x, y, 0, 0);
            }
        }
    }
    
    return result;
}

// ========== Morphological Operations ==========

std::vector<std::vector<int>> ImageProcessor::createStructuringElement(int size, StructElement element) {
    if (size % 2 == 0) size++;
    
    std::vector<std::vector<int>> kernel(size, std::vector<int>(size, 0));
    int center = size / 2;
    
    switch (element) {
        case StructElement::RECT:
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    kernel[i][j] = 1;
                }
            }
            break;
            
        case StructElement::CROSS:
            for (int i = 0; i < size; i++) {
                kernel[i][center] = 1;
                kernel[center][i] = 1;
            }
            break;
            
        case StructElement::ELLIPSE:
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    int x = j - center;
                    int y = i - center;
                    if (x*x + y*y <= center*center) {
                        kernel[i][j] = 1;
                    }
                }
            }
            break;
    }
    
    return kernel;
}

Image ImageProcessor::morphology(const Image& img, MorphOp op, int kernelSize, StructElement element) {
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    auto kernel = createStructuringElement(kernelSize, element);
    
    int offset = kernelSize / 2;
    Image result = gray.clone();
    
    switch (op) {
        case MorphOp::ERODE: {
            for (int y = 0; y < gray.getHeight(); y++) {
                for (int x = 0; x < gray.getWidth(); x++) {
                    uint8_t minVal = 255;
                    
                    for (int ky = 0; ky < kernelSize; ky++) {
                        for (int kx = 0; kx < kernelSize; kx++) {
                            if (kernel[ky][kx] == 0) continue;
                            
                            int px = std::max(0, std::min(gray.getWidth() - 1, x + kx - offset));
                            int py = std::max(0, std::min(gray.getHeight() - 1, y + ky - offset));
                            minVal = std::min(minVal, gray.getPixel(px, py, 0));
                        }
                    }
                    
                    result.setPixel(x, y, 0, minVal);
                }
            }
            break;
        }
        
        case MorphOp::DILATE: {
            for (int y = 0; y < gray.getHeight(); y++) {
                for (int x = 0; x < gray.getWidth(); x++) {
                    uint8_t maxVal = 0;
                    
                    for (int ky = 0; ky < kernelSize; ky++) {
                        for (int kx = 0; kx < kernelSize; kx++) {
                            if (kernel[ky][kx] == 0) continue;
                            
                            int px = std::max(0, std::min(gray.getWidth() - 1, x + kx - offset));
                            int py = std::max(0, std::min(gray.getHeight() - 1, y + ky - offset));
                            maxVal = std::max(maxVal, gray.getPixel(px, py, 0));
                        }
                    }
                    
                    result.setPixel(x, y, 0, maxVal);
                }
            }
            break;
        }
        
        case MorphOp::OPEN: {
            // Opening = Erosion followed by Dilation
            Image eroded = morphology(gray, MorphOp::ERODE, kernelSize, element);
            result = morphology(eroded, MorphOp::DILATE, kernelSize, element);
            break;
        }
        
        case MorphOp::CLOSE: {
            // Closing = Dilation followed by Erosion
            Image dilated = morphology(gray, MorphOp::DILATE, kernelSize, element);
            result = morphology(dilated, MorphOp::ERODE, kernelSize, element);
            break;
        }
    }
    
    return result;
}
