#include "Histogram.h"
#include <algorithm>
#include <cmath>

std::array<int, 256> Histogram::compute(const Image& img, int channel) {
    std::array<int, 256> hist = {0};

    if (channel == -1) {
        for (int y = 0; y < img.getHeight(); y++) {
            for (int x = 0; x < img.getWidth(); x++) {
                glm::vec3 rgb = img.getPixelRGB(x, y);
                int lum = static_cast<int>((0.299f * rgb.r + 0.587f * rgb.g + 0.114f * rgb.b) * 255);
                lum = glm::clamp(lum, 0, 255);
                hist[lum]++;
            }
        }
    } else {
        for (int y = 0; y < img.getHeight(); y++) {
            for (int x = 0; x < img.getWidth(); x++) {
                unsigned char val = img.getPixel(x, y, channel);
                hist[val]++;
            }
        }
    }
    
    return hist;
}

std::array<int, 256> Histogram::computeLuminance(const Image& img) {
    return compute(img, -1);
}

Image Histogram::equalizeRGB(const Image& img) {
    Image result = img.clone();

    for (int c = 0; c < std::min(3, img.getChannels()); c++) {
        auto hist = compute(img, c);

        std::array<int, 256> cdf = {0};
        cdf[0] = hist[0];
        for (int i = 1; i < 256; i++) {
            cdf[i] = cdf[i-1] + hist[i];
        }

        int cdfMin = cdf[0];
        for (int i = 0; i < 256; i++) {
            if (cdf[i] > 0) {
                cdfMin = cdf[i];
                break;
            }
        }
        
        int totalPixels = img.getWidth() * img.getHeight();
        std::array<unsigned char, 256> lut;
        
        for (int i = 0; i < 256; i++) {
            lut[i] = static_cast<unsigned char>(
                std::round(((cdf[i] - cdfMin) / static_cast<float>(totalPixels - cdfMin)) * 255.0f)
            );
        }

        for (int y = 0; y < img.getHeight(); y++) {
            for (int x = 0; x < img.getWidth(); x++) {
                unsigned char oldVal = img.getPixel(x, y, c);
                result.setPixel(x, y, c, lut[oldVal]);
            }
        }
    }
    
    result.updateTexture();
    return result;
}

Image Histogram::equalizeHSV(const Image& img) {
    Image result = img.clone();

    std::array<int, 256> hist = {0};
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            glm::vec3 rgb = img.getPixelRGB(x, y);
            glm::vec3 hsv = RGBtoHSV(rgb);
            int v = static_cast<int>(hsv.z * 255);
            v = glm::clamp(v, 0, 255);
            hist[v]++;
        }
    }

    std::array<int, 256> cdf = {0};
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + hist[i];
    }

    int cdfMin = cdf[0];
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdfMin = cdf[i];
            break;
        }
    }
    
    int totalPixels = img.getWidth() * img.getHeight();
    std::array<unsigned char, 256> lut;
    
    for (int i = 0; i < 256; i++) {
        lut[i] = static_cast<unsigned char>(
            std::round(((cdf[i] - cdfMin) / static_cast<float>(totalPixels - cdfMin)) * 255.0f)
        );
    }

    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            glm::vec3 rgb = img.getPixelRGB(x, y);
            glm::vec3 hsv = RGBtoHSV(rgb);
            
            int oldV = static_cast<int>(hsv.z * 255);
            oldV = glm::clamp(oldV, 0, 255);
            hsv.z = lut[oldV] / 255.0f;
            
            glm::vec3 newRgb = HSVtoRGB(hsv);
            result.setPixelRGB(x, y, newRgb);
        }
    }
    
    result.updateTexture();
    return result;
}

Image Histogram::linearContrast(const Image& img, float minPercentile, float maxPercentile) {
    auto hist = computeLuminance(img);
    
    int totalPixels = img.getWidth() * img.getHeight();
    int minCount = static_cast<int>(totalPixels * minPercentile / 100.0f);
    int maxCount = static_cast<int>(totalPixels * maxPercentile / 100.0f);

    int cumSum = 0;
    unsigned char minVal = 0, maxVal = 255;
    
    for (int i = 0; i < 256; i++) {
        cumSum += hist[i];
        if (cumSum >= minCount) {
            minVal = i;
            break;
        }
    }
    
    cumSum = 0;
    for (int i = 255; i >= 0; i--) {
        cumSum += hist[i];
        if (cumSum >= totalPixels - maxCount) {
            maxVal = i;
            break;
        }
    }
    
    return linearContrastManual(img, minVal, maxVal);
}

Image Histogram::linearContrastManual(const Image& img, unsigned char minIn, unsigned char maxIn) {
    Image result = img.clone();
    
    if (minIn >= maxIn) {
        return result;
    }
    
    float scale = 255.0f / (maxIn - minIn);
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < std::min(3, img.getChannels()); c++) {
                unsigned char val = img.getPixel(x, y, c);
                
                int newVal;
                if (val <= minIn) {
                    newVal = 0;
                } else if (val >= maxIn) {
                    newVal = 255;
                } else {
                    newVal = static_cast<int>((val - minIn) * scale);
                }
                
                result.setPixel(x, y, c, glm::clamp(newVal, 0, 255));
            }
        }
    }
    
    result.updateTexture();
    return result;
}

glm::vec3 Histogram::RGBtoHSV(const glm::vec3& rgb) {
    float r = rgb.r, g = rgb.g, b = rgb.b;
    float maxC = std::max({r, g, b});
    float minC = std::min({r, g, b});
    float delta = maxC - minC;
    
    float h = 0, s = 0, v = maxC;
    
    if (delta > 0.00001f) {
        s = delta / maxC;
        
        if (maxC == r) {
            h = 60.0f * fmod((g - b) / delta, 6.0f);
        } else if (maxC == g) {
            h = 60.0f * ((b - r) / delta + 2.0f);
        } else {
            h = 60.0f * ((r - g) / delta + 4.0f);
        }
        
        if (h < 0) h += 360.0f;
    }
    
    return glm::vec3(h, s, v);
}

glm::vec3 Histogram::HSVtoRGB(const glm::vec3& hsv) {
    float h = hsv.x, s = hsv.y, v = hsv.z;
    
    float c = v * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    
    float r = 0, g = 0, b = 0;
    
    if (h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if (h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if (h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if (h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    return glm::vec3(r + m, g + m, b + m);
}