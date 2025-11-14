#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <cstring>
#include <iostream>

Image::Image() : width(0), height(0), channels(0) {}

Image::Image(int w, int h, int c) : width(w), height(h), channels(c) {
    data.resize(width * height * channels, 0);
}

Image::~Image() {}

bool Image::load(const std::string& filename) {
    int w, h, c;
    uint8_t* loaded_data = stbi_load(filename.c_str(), &w, &h, &c, 0);
    
    if (!loaded_data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    width = w;
    height = h;
    channels = c;
    
    size_t size = width * height * channels;
    data.resize(size);
    std::memcpy(data.data(), loaded_data, size);
    
    stbi_image_free(loaded_data);
    
    std::cout << "Loaded image: " << filename << " (" << width << "x" << height 
              << ", " << channels << " channels)" << std::endl;
    
    return true;
}

bool Image::save(const std::string& filename) const {
    if (data.empty()) {
        std::cerr << "Cannot save empty image" << std::endl;
        return false;
    }
    
    // Determine format from extension
    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    int result = 0;
    if (ext == "png") {
        result = stbi_write_png(filename.c_str(), width, height, channels, data.data(), width * channels);
    } else if (ext == "jpg" || ext == "jpeg") {
        result = stbi_write_jpg(filename.c_str(), width, height, channels, data.data(), 90);
    } else if (ext == "bmp") {
        result = stbi_write_bmp(filename.c_str(), width, height, channels, data.data());
    } else {
        std::cerr << "Unsupported file format: " << ext << std::endl;
        return false;
    }
    
    if (!result) {
        std::cerr << "Failed to save image: " << filename << std::endl;
        return false;
    }
    
    std::cout << "Saved image: " << filename << std::endl;
    return true;
}

uint8_t Image::getPixel(int x, int y, int channel) const {
    if (!isValidCoord(x, y) || channel < 0 || channel >= channels) {
        return 0;
    }
    return data[(y * width + x) * channels + channel];
}

void Image::setPixel(int x, int y, int channel, uint8_t value) {
    if (!isValidCoord(x, y) || channel < 0 || channel >= channels) {
        return;
    }
    data[(y * width + x) * channels + channel] = value;
}

Image Image::clone() const {
    Image result(width, height, channels);
    result.data = data;
    return result;
}

void Image::copyFrom(const Image& other) {
    width = other.width;
    height = other.height;
    channels = other.channels;
    data = other.data;
}

Image Image::toGrayscale() const {
    if (channels == 1) {
        return clone();
    }
    
    Image gray(width, height, 1);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Use standard luminance conversion
            float r = getPixel(x, y, 0) / 255.0f;
            float g = channels > 1 ? getPixel(x, y, 1) / 255.0f : r;
            float b = channels > 2 ? getPixel(x, y, 2) / 255.0f : r;
            
            float luminance = 0.299f * r + 0.587f * g + 0.114f * b;
            uint8_t grayValue = static_cast<uint8_t>(luminance * 255.0f);
            
            gray.setPixel(x, y, 0, grayValue);
        }
    }
    
    return gray;
}

bool Image::isValidCoord(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}
