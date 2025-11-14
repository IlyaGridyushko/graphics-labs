#include "PointOperations.h"
#include <algorithm>
#include <cmath>
#include <vector>

Image PointOperations::linearContrast(const Image& img, float minPercentile, float maxPercentile) {
    std::vector<unsigned char> intensities;
    intensities.reserve(img.getWidth() * img.getHeight());
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            if (img.getChannels() >= 3) {
                glm::vec3 rgb = img.getPixelRGB(x, y);
                unsigned char intensity = static_cast<unsigned char>(
                    (0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b) * 255
                );
                intensities.push_back(intensity);
            } else {
                intensities.push_back(img.getPixel(x, y, 0));
            }
        }
    }

    std::sort(intensities.begin(), intensities.end());
    
    int minIdx = static_cast<int>(intensities.size() * minPercentile / 100.0f);
    int maxIdx = static_cast<int>(intensities.size() * maxPercentile / 100.0f);
    
    unsigned char minVal = intensities[minIdx];
    unsigned char maxVal = intensities[maxIdx];
    
    return linearContrastManual(img, minVal, maxVal);
}

Image PointOperations::linearContrastManual(const Image& img, unsigned char minIn, unsigned char maxIn,
                                           unsigned char minOut, unsigned char maxOut) {
    Image result = img.clone();
    
    if (minIn >= maxIn) {
        return result;
    }
    
    float scale = static_cast<float>(maxOut - minOut) / (maxIn - minIn);
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);
                
                int newVal;
                if (val <= minIn) {
                    newVal = minOut;
                } else if (val >= maxIn) {
                    newVal = maxOut;
                } else {
                    newVal = static_cast<int>(minOut + (val - minIn) * scale);
                }
                
                result.setPixel(x, y, c, glm::clamp(newVal, 0, 255));
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::adjustBrightnessContrast(const Image& img, float brightness, float contrast) {
    Image result = img.clone();

    float factor = (259.0f * (contrast + 255.0f)) / (255.0f * (259.0f - contrast));
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);

                float newVal = factor * (val - 128.0f) + 128.0f;

                newVal += brightness;
                
                result.setPixel(x, y, c, glm::clamp(static_cast<int>(newVal), 0, 255));
            }
        }
    }
    
    result.updateTexture();
    return result;
}

unsigned char PointOperations::applyGamma(unsigned char value, float gamma) {
    float normalized = value / 255.0f;
    float corrected = std::pow(normalized, gamma);
    return static_cast<unsigned char>(corrected * 255.0f);
}

Image PointOperations::gammaCorrection(const Image& img, float gamma) {
    Image result = img.clone();

    unsigned char lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = applyGamma(i, gamma);
    }
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);
                result.setPixel(x, y, c, lut[val]);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

unsigned char PointOperations::applyLog(unsigned char value, float c) {
    float result = c * std::log(1.0f + value);
    return static_cast<unsigned char>(glm::clamp(result, 0.0f, 255.0f));
}

Image PointOperations::logarithmicTransform(const Image& img, float c) {
    Image result = img.clone();

    float maxLog = std::log(256.0f);
    float normalizedC = 255.0f / maxLog * c;
    
    unsigned char lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = applyLog(i, normalizedC);
    }
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);
                result.setPixel(x, y, c, lut[val]);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::powerTransform(const Image& img, float power, float c) {
    Image result = img.clone();
    
    unsigned char lut[256];
    for (int i = 0; i < 256; i++) {
        float normalized = i / 255.0f;
        float transformed = c * std::pow(normalized, power);
        lut[i] = static_cast<unsigned char>(glm::clamp(transformed * 255.0f, 0.0f, 255.0f));
    }
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);
                result.setPixel(x, y, c, lut[val]);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::invert(const Image& img) {
    Image result = img.clone();
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);
                result.setPixel(x, y, c, 255 - val);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::clipBrightness(const Image& img, unsigned char minVal, unsigned char maxVal) {
    Image result = img.clone();
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);
                result.setPixel(x, y, c, glm::clamp(val, minVal, maxVal));
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::quantize(const Image& img, int levels) {
    Image result = img.clone();
    
    if (levels <= 1) levels = 2;
    if (levels > 256) levels = 256;
    
    float step = 255.0f / (levels - 1);
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                unsigned char val = img.getPixel(x, y, c);
                int level = static_cast<int>(std::round(val / step));
                unsigned char newVal = static_cast<unsigned char>(level * step);
                result.setPixel(x, y, c, newVal);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::bitwiseAND(const Image& img1, const Image& img2) {
    if (img1.getWidth() != img2.getWidth() || img1.getHeight() != img2.getHeight()) {
        return img1.clone();
    }
    
    Image result = img1.clone();
    
    for (int y = 0; y < img1.getHeight(); y++) {
        for (int x = 0; x < img1.getWidth(); x++) {
            for (int c = 0; c < std::min(img1.getChannels(), img2.getChannels()); c++) {
                unsigned char val1 = img1.getPixel(x, y, c);
                unsigned char val2 = img2.getPixel(x, y, c);
                result.setPixel(x, y, c, val1 & val2);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::bitwiseOR(const Image& img1, const Image& img2) {
    if (img1.getWidth() != img2.getWidth() || img1.getHeight() != img2.getHeight()) {
        return img1.clone();
    }
    
    Image result = img1.clone();
    
    for (int y = 0; y < img1.getHeight(); y++) {
        for (int x = 0; x < img1.getWidth(); x++) {
            for (int c = 0; c < std::min(img1.getChannels(), img2.getChannels()); c++) {
                unsigned char val1 = img1.getPixel(x, y, c);
                unsigned char val2 = img2.getPixel(x, y, c);
                result.setPixel(x, y, c, val1 | val2);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::bitwiseXOR(const Image& img1, const Image& img2) {
    if (img1.getWidth() != img2.getWidth() || img1.getHeight() != img2.getHeight()) {
        return img1.clone();
    }
    
    Image result = img1.clone();
    
    for (int y = 0; y < img1.getHeight(); y++) {
        for (int x = 0; x < img1.getWidth(); x++) {
            for (int c = 0; c < std::min(img1.getChannels(), img2.getChannels()); c++) {
                unsigned char val1 = img1.getPixel(x, y, c);
                unsigned char val2 = img2.getPixel(x, y, c);
                result.setPixel(x, y, c, val1 ^ val2);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image PointOperations::bitwiseNOT(const Image& img) {
    return invert(img);
}