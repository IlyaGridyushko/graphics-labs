#include "ThresholdProcessing.h"
#include <algorithm>
#include <cmath>
#include <limits>

std::array<int, 256> ThresholdProcessing::computeHistogram(const Image& img) {
    std::array<int, 256> hist = {0};
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            if (img.getChannels() >= 3) {
                glm::vec3 rgb = img.getPixelRGB(x, y);
                int intensity = static_cast<int>((0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b) * 255);
                intensity = glm::clamp(intensity, 0, 255);
                hist[intensity]++;
            } else {
                hist[img.getPixel(x, y, 0)]++;
            }
        }
    }
    
    return hist;
}

unsigned char ThresholdProcessing::calculateOtsuThreshold(const Image& img) {
    auto hist = computeHistogram(img);
    int totalPixels = img.getWidth() * img.getHeight();
    
    float sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += i * hist[i];
    }
    
    float sumB = 0;
    int wB = 0;
    int wF = 0;
    
    float maxVariance = 0;
    unsigned char threshold = 0;
    
    for (int t = 0; t < 256; t++) {
        wB += hist[t];
        if (wB == 0) continue;

        wF = totalPixels - wB;
        if (wF == 0) break;

        sumB += t * hist[t];

        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;

        float variance = wB * wF * (mB - mF) * (mB - mF);
        
        if (variance > maxVariance) {
            maxVariance = variance;
            threshold = t;
        }
    }
    
    return threshold;
}

Image ThresholdProcessing::otsuThreshold(const Image& img) {
    unsigned char threshold = calculateOtsuThreshold(img);
    return fixedThreshold(img, threshold);
}

unsigned char ThresholdProcessing::calculateTriangleThreshold(const Image& img) {
    auto hist = computeHistogram(img);

    int maxIdx = 0;
    int maxVal = hist[0];
    for (int i = 1; i < 256; i++) {
        if (hist[i] > maxVal) {
            maxVal = hist[i];
            maxIdx = i;
        }
    }

    int first = 0, last = 255;
    for (int i = 0; i < 256; i++) {
        if (hist[i] > 0) {
            first = i;
            break;
        }
    }
    for (int i = 255; i >= 0; i--) {
        if (hist[i] > 0) {
            last = i;
            break;
        }
    }

    bool invertedTriangle = (maxIdx - first) < (last - maxIdx);

    int lineStart = invertedTriangle ? last : first;
    int lineEnd = maxIdx;

    float maxDistance = 0;
    int threshold = maxIdx;

    for (int i = std::min(lineStart, lineEnd); i <= std::max(lineStart, lineEnd); i++) {
        float distance = std::abs(
            static_cast<float>((lineEnd - lineStart) * hist[i] - 
            (maxVal - hist[lineStart]) * (i - lineStart) - 
            (lineEnd - lineStart) * hist[lineStart])
        ) / std::sqrt(
            static_cast<float>((lineEnd - lineStart) * (lineEnd - lineStart) + 
            (maxVal - hist[lineStart]) * (maxVal - hist[lineStart]))
        );
        
        if (distance > maxDistance) {
            maxDistance = distance;
            threshold = i;
        }
    }
    
    return static_cast<unsigned char>(threshold);
}

Image ThresholdProcessing::triangleThreshold(const Image& img) {
    unsigned char threshold = calculateTriangleThreshold(img);
    return fixedThreshold(img, threshold);
}

Image ThresholdProcessing::fixedThreshold(const Image& img, unsigned char threshold) {
    Image result = img.clone();
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            unsigned char intensity;
            
            if (img.getChannels() >= 3) {
                glm::vec3 rgb = img.getPixelRGB(x, y);
                intensity = static_cast<unsigned char>((0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b) * 255);
            } else {
                intensity = img.getPixel(x, y, 0);
            }
            
            unsigned char value = (intensity >= threshold) ? 255 : 0;

            for (int c = 0; c < img.getChannels(); c++) {
                result.setPixel(x, y, c, value);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image ThresholdProcessing::doubleThreshold(const Image& img, unsigned char lowThreshold, unsigned char highThreshold) {
    Image result = img.clone();
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            unsigned char intensity;
            
            if (img.getChannels() >= 3) {
                glm::vec3 rgb = img.getPixelRGB(x, y);
                intensity = static_cast<unsigned char>((0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b) * 255);
            } else {
                intensity = img.getPixel(x, y, 0);
            }
            
            unsigned char value;
            if (intensity >= highThreshold) {
                value = 255;
            } else if (intensity >= lowThreshold) {
                value = 128;
            } else {
                value = 0;
            }
            
            for (int c = 0; c < img.getChannels(); c++) {
                result.setPixel(x, y, c, value);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

float ThresholdProcessing::calculateImageIntensity(const Image& img) {
    float sum = 0;
    int count = img.getWidth() * img.getHeight();
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            if (img.getChannels() >= 3) {
                glm::vec3 rgb = img.getPixelRGB(x, y);
                sum += (0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b) * 255;
            } else {
                sum += img.getPixel(x, y, 0);
            }
        }
    }
    
    return sum / count;
}