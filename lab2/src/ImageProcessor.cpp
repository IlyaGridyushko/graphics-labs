#include "ImageProcessor.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

// ========== Histogram Operations ==========

std::vector<int> ImageProcessor::calculateHistogram(const Image& img, int channel) {
    std::vector<int> histogram(256, 0);
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            uint8_t value = img.getPixel(x, y, channel);
            histogram[value]++;
        }
    }
    
    return histogram;
}

Image ImageProcessor::equalizeHistogram(const Image& img) {
    // Convert to grayscale if needed
    Image gray = img.getChannels() == 1 ? img.clone() : img.toGrayscale();
    
    std::vector<int> histogram = calculateHistogram(gray, 0);
    
    // Calculate cumulative distribution function
    std::vector<int> cdf(256, 0);
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + histogram[i];
    }
    
    // Find minimum non-zero CDF value
    int cdfMin = cdf[0];
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdfMin = cdf[i];
            break;
        }
    }
    
    // Create lookup table
    std::vector<uint8_t> lut(256);
    int totalPixels = gray.getWidth() * gray.getHeight();
    for (int i = 0; i < 256; i++) {
        lut[i] = static_cast<uint8_t>(
            std::round(((cdf[i] - cdfMin) / static_cast<float>(totalPixels - cdfMin)) * 255.0f)
        );
    }
    
    // Apply lookup table
    Image result = gray.clone();
    for (int y = 0; y < result.getHeight(); y++) {
        for (int x = 0; x < result.getWidth(); x++) {
            uint8_t oldValue = result.getPixel(x, y, 0);
            result.setPixel(x, y, 0, lut[oldValue]);
        }
    }
    
    return result;
}

Image ImageProcessor::equalizeHistogramRGB(const Image& img) {
    if (img.getChannels() < 3) {
        return equalizeHistogram(img);
    }
    
    Image result = img.clone();
    
    for (int channel = 0; channel < 3; channel++) {
        std::vector<int> histogram = calculateHistogram(img, channel);
        
        // Calculate CDF
        std::vector<int> cdf(256, 0);
        cdf[0] = histogram[0];
        for (int i = 1; i < 256; i++) {
            cdf[i] = cdf[i-1] + histogram[i];
        }
        
        int cdfMin = cdf[0];
        for (int i = 0; i < 256; i++) {
            if (cdf[i] > 0) {
                cdfMin = cdf[i];
                break;
            }
        }
        
        // Create lookup table
        std::vector<uint8_t> lut(256);
        int totalPixels = img.getWidth() * img.getHeight();
        for (int i = 0; i < 256; i++) {
            lut[i] = static_cast<uint8_t>(
                std::round(((cdf[i] - cdfMin) / static_cast<float>(totalPixels - cdfMin)) * 255.0f)
            );
        }
        
        // Apply lookup table
        for (int y = 0; y < result.getHeight(); y++) {
            for (int x = 0; x < result.getWidth(); x++) {
                uint8_t oldValue = result.getPixel(x, y, channel);
                result.setPixel(x, y, channel, lut[oldValue]);
            }
        }
    }
    
    return result;
}

Image ImageProcessor::equalizeHistogramHSV(const Image& img) {
    if (img.getChannels() < 3) {
        return equalizeHistogram(img);
    }
    
    Image result = img.clone();
    
    // Convert to HSV, equalize V channel, convert back
    std::vector<float> vChannel;
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            uint8_t r = img.getPixel(x, y, 0);
            uint8_t g = img.getPixel(x, y, 1);
            uint8_t b = img.getPixel(x, y, 2);
            float h, s, v;
            rgbToHsv(r, g, b, h, s, v);
            vChannel.push_back(v);
        }
    }
    
    // Build histogram of V channel (0-255)
    std::vector<int> histogram(256, 0);
    for (float v : vChannel) {
        int idx = std::min(255, static_cast<int>(v * 255.0f));
        histogram[idx]++;
    }
    
    // Calculate CDF
    std::vector<int> cdf(256, 0);
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + histogram[i];
    }
    
    int cdfMin = cdf[0];
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdfMin = cdf[i];
            break;
        }
    }
    
    // Create lookup table
    std::vector<float> lut(256);
    int totalPixels = img.getWidth() * img.getHeight();
    for (int i = 0; i < 256; i++) {
        lut[i] = ((cdf[i] - cdfMin) / static_cast<float>(totalPixels - cdfMin));
    }
    
    // Apply equalization and convert back to RGB
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            uint8_t r = img.getPixel(x, y, 0);
            uint8_t g = img.getPixel(x, y, 1);
            uint8_t b = img.getPixel(x, y, 2);
            
            float h, s, v;
            rgbToHsv(r, g, b, h, s, v);
            
            int vIdx = std::min(255, static_cast<int>(v * 255.0f));
            v = lut[vIdx];
            
            hsvToRgb(h, s, v, r, g, b);
            
            result.setPixel(x, y, 0, r);
            result.setPixel(x, y, 1, g);
            result.setPixel(x, y, 2, b);
        }
    }
    
    return result;
}

// ========== Contrast Enhancement ==========

Image ImageProcessor::linearContrast(const Image& img, float alpha, float beta) {
    Image result = img.clone();
    
    for (int y = 0; y < result.getHeight(); y++) {
        for (int x = 0; x < result.getWidth(); x++) {
            for (int c = 0; c < result.getChannels(); c++) {
                float value = img.getPixel(x, y, c);
                float newValue = alpha * value + beta;
                newValue = std::max(0.0f, std::min(255.0f, newValue));
                result.setPixel(x, y, c, static_cast<uint8_t>(newValue));
            }
        }
    }
    
    return result;
}

Image ImageProcessor::autoLinearContrast(const Image& img) {
    // Find min and max values for each channel
    Image result = img.clone();
    
    for (int c = 0; c < img.getChannels(); c++) {
        uint8_t minVal = 255, maxVal = 0;
        
        for (int y = 0; y < img.getHeight(); y++) {
            for (int x = 0; x < img.getWidth(); x++) {
                uint8_t val = img.getPixel(x, y, c);
                minVal = std::min(minVal, val);
                maxVal = std::max(maxVal, val);
            }
        }
        
        if (maxVal > minVal) {
            for (int y = 0; y < result.getHeight(); y++) {
                for (int x = 0; x < result.getWidth(); x++) {
                    float value = img.getPixel(x, y, c);
                    float newValue = ((value - minVal) / (maxVal - minVal)) * 255.0f;
                    result.setPixel(x, y, c, static_cast<uint8_t>(newValue));
                }
            }
        }
    }
    
    return result;
}

// ========== Low-pass Filters (Smoothing) ==========

Image ImageProcessor::averageFilter(const Image& img, int kernelSize) {
    if (kernelSize % 2 == 0) kernelSize++;  // Ensure odd size
    
    std::vector<std::vector<float>> kernel(kernelSize, std::vector<float>(kernelSize));
    float value = 1.0f / (kernelSize * kernelSize);
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            kernel[i][j] = value;
        }
    }
    
    return applyConvolution(img, kernel);
}

std::vector<std::vector<float>> ImageProcessor::createGaussianKernel(int size, float sigma) {
    if (size % 2 == 0) size++;  // Ensure odd size
    
    std::vector<std::vector<float>> kernel(size, std::vector<float>(size));
    float sum = 0.0f;
    int center = size / 2;
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int x = i - center;
            int y = j - center;
            float value = std::exp(-(x*x + y*y) / (2.0f * sigma * sigma));
            kernel[i][j] = value;
            sum += value;
        }
    }
    
    // Normalize
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] /= sum;
        }
    }
    
    return kernel;
}

Image ImageProcessor::gaussianFilter(const Image& img, int kernelSize, float sigma) {
    std::vector<std::vector<float>> kernel = createGaussianKernel(kernelSize, sigma);
    return applyConvolution(img, kernel);
}

// ========== High-pass Filters (Sharpening) ==========

Image ImageProcessor::sharpenFilter(const Image& img) {
    std::vector<std::vector<float>> kernel = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    return applyConvolution(img, kernel);
}

Image ImageProcessor::laplacianFilter(const Image& img) {
    std::vector<std::vector<float>> kernel = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    return applyConvolution(img, kernel);
}

Image ImageProcessor::unsharpMasking(const Image& img, float amount) {
    Image blurred = gaussianFilter(img, 5, 1.0f);
    Image result = img.clone();
    
    for (int y = 0; y < result.getHeight(); y++) {
        for (int x = 0; x < result.getWidth(); x++) {
            for (int c = 0; c < result.getChannels(); c++) {
                float original = img.getPixel(x, y, c);
                float blur = blurred.getPixel(x, y, c);
                float sharp = original + amount * (original - blur);
                sharp = std::max(0.0f, std::min(255.0f, sharp));
                result.setPixel(x, y, c, static_cast<uint8_t>(sharp));
            }
        }
    }
    
    return result;
}

// ========== Non-linear Filters ==========

uint8_t ImageProcessor::getMedian(std::vector<uint8_t>& values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

Image ImageProcessor::medianFilter(const Image& img, int kernelSize) {
    if (kernelSize % 2 == 0) kernelSize++;
    
    Image result = img.clone();
    int offset = kernelSize / 2;
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                std::vector<uint8_t> values;
                
                for (int ky = -offset; ky <= offset; ky++) {
                    for (int kx = -offset; kx <= offset; kx++) {
                        int px = std::max(0, std::min(img.getWidth() - 1, x + kx));
                        int py = std::max(0, std::min(img.getHeight() - 1, y + ky));
                        values.push_back(img.getPixel(px, py, c));
                    }
                }
                
                result.setPixel(x, y, c, getMedian(values));
            }
        }
    }
    
    return result;
}

Image ImageProcessor::minFilter(const Image& img, int kernelSize) {
    if (kernelSize % 2 == 0) kernelSize++;
    
    Image result = img.clone();
    int offset = kernelSize / 2;
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                uint8_t minVal = 255;
                
                for (int ky = -offset; ky <= offset; ky++) {
                    for (int kx = -offset; kx <= offset; kx++) {
                        int px = std::max(0, std::min(img.getWidth() - 1, x + kx));
                        int py = std::max(0, std::min(img.getHeight() - 1, y + ky));
                        minVal = std::min(minVal, img.getPixel(px, py, c));
                    }
                }
                
                result.setPixel(x, y, c, minVal);
            }
        }
    }
    
    return result;
}

Image ImageProcessor::maxFilter(const Image& img, int kernelSize) {
    if (kernelSize % 2 == 0) kernelSize++;
    
    Image result = img.clone();
    int offset = kernelSize / 2;
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                uint8_t maxVal = 0;
                
                for (int ky = -offset; ky <= offset; ky++) {
                    for (int kx = -offset; kx <= offset; kx++) {
                        int px = std::max(0, std::min(img.getWidth() - 1, x + kx));
                        int py = std::max(0, std::min(img.getHeight() - 1, y + ky));
                        maxVal = std::max(maxVal, img.getPixel(px, py, c));
                    }
                }
                
                result.setPixel(x, y, c, maxVal);
            }
        }
    }
    
    return result;
}

// ========== Helper Functions ==========

Image ImageProcessor::applyConvolution(const Image& img, const std::vector<std::vector<float>>& kernel) {
    Image result = img.clone();
    int kernelSize = kernel.size();
    int offset = kernelSize / 2;
    
    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            for (int c = 0; c < img.getChannels(); c++) {
                float sum = 0.0f;
                
                for (int ky = 0; ky < kernelSize; ky++) {
                    for (int kx = 0; kx < kernelSize; kx++) {
                        int px = std::max(0, std::min(img.getWidth() - 1, x + kx - offset));
                        int py = std::max(0, std::min(img.getHeight() - 1, y + ky - offset));
                        sum += img.getPixel(px, py, c) * kernel[ky][kx];
                    }
                }
                
                sum = std::max(0.0f, std::min(255.0f, sum));
                result.setPixel(x, y, c, static_cast<uint8_t>(sum));
            }
        }
    }
    
    return result;
}

void ImageProcessor::rgbToHsv(uint8_t r, uint8_t g, uint8_t b, float& h, float& s, float& v) {
    float rf = r / 255.0f;
    float gf = g / 255.0f;
    float bf = b / 255.0f;
    
    float maxVal = std::max({rf, gf, bf});
    float minVal = std::min({rf, gf, bf});
    float delta = maxVal - minVal;
    
    v = maxVal;
    
    if (delta < 0.00001f) {
        s = 0.0f;
        h = 0.0f;
        return;
    }
    
    if (maxVal > 0.0f) {
        s = delta / maxVal;
    } else {
        s = 0.0f;
        h = 0.0f;
        return;
    }
    
    if (rf >= maxVal) {
        h = (gf - bf) / delta;
    } else if (gf >= maxVal) {
        h = 2.0f + (bf - rf) / delta;
    } else {
        h = 4.0f + (rf - gf) / delta;
    }
    
    h *= 60.0f;
    if (h < 0.0f) h += 360.0f;
}

void ImageProcessor::hsvToRgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    if (s <= 0.0f) {
        r = g = b = static_cast<uint8_t>(v * 255.0f);
        return;
    }
    
    float hh = h;
    if (hh >= 360.0f) hh = 0.0f;
    hh /= 60.0f;
    
    int i = static_cast<int>(hh);
    float ff = hh - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - (s * ff));
    float t = v * (1.0f - (s * (1.0f - ff)));
    
    float rf, gf, bf;
    
    switch (i) {
        case 0:
            rf = v; gf = t; bf = p;
            break;
        case 1:
            rf = q; gf = v; bf = p;
            break;
        case 2:
            rf = p; gf = v; bf = t;
            break;
        case 3:
            rf = p; gf = q; bf = v;
            break;
        case 4:
            rf = t; gf = p; bf = v;
            break;
        case 5:
        default:
            rf = v; gf = p; bf = q;
            break;
    }
    
    r = static_cast<uint8_t>(rf * 255.0f);
    g = static_cast<uint8_t>(gf * 255.0f);
    b = static_cast<uint8_t>(bf * 255.0f);
}
