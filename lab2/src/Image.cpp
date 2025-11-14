#include "Image.h"
#include <iostream>
#include <cstring>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <glad/glad.h>

Image::Image() : width(0), height(0), channels(0), data(nullptr), textureID(0) {}

Image::Image(int w, int h, int c) : width(w), height(h), channels(c), textureID(0) {
    data = new unsigned char[width * height * channels];
    std::memset(data, 0, width * height * channels);
    createTexture();
}

Image::~Image() {
    delete[] data;
    deleteTexture();
}

Image::Image(const Image& other)
    : width(other.width), height(other.height), channels(other.channels), textureID(0) {
    if (other.data) {
        data = new unsigned char[width * height * channels];
        std::memcpy(data, other.data, width * height * channels);
        createTexture();
        updateTexture();
    } else {
        data = nullptr;
    }
}

Image& Image::operator=(const Image& other) {
    if (this != &other) {
        delete[] data;
        deleteTexture();

        width = other.width;
        height = other.height;
        channels = other.channels;

        if (other.data) {
            data = new unsigned char[width * height * channels];
            std::memcpy(data, other.data, width * height * channels);
            createTexture();
            updateTexture();
        } else {
            data = nullptr;
            textureID = 0;
        }
    }
    return *this;
}

Image::Image(Image&& other) noexcept
    : width(other.width), height(other.height), channels(other.channels),
      data(other.data), textureID(other.textureID) {
    other.data = nullptr;
    other.textureID = 0;
    other.width = 0;
    other.height = 0;
    other.channels = 0;
}

Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) {
        delete[] data;
        deleteTexture();

        width = other.width;
        height = other.height;
        channels = other.channels;
        data = other.data;
        textureID = other.textureID;

        other.data = nullptr;
        other.textureID = 0;
        other.width = 0;
        other.height = 0;
        other.channels = 0;
    }
    return *this;
}

bool Image::load(const std::string& filepath) {
    if (data) {
        delete[] data;
        data = nullptr;
    }
    
    data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load image: " << filepath << std::endl;
        return false;
    }
    
    createTexture();
    updateTexture();
    
    return true;
}

bool Image::save(const std::string& filepath) {
    if (!data) return false;
    
    return stbi_write_png(filepath.c_str(), width, height, channels, data, width * channels);
}

void Image::setPixel(int x, int y, int channel, unsigned char value) {
    if (x >= 0 && x < width && y >= 0 && y < height && channel >= 0 && channel < channels) {
        data[(y * width + x) * channels + channel] = value;
    }
}

unsigned char Image::getPixel(int x, int y, int channel) const {
    if (x >= 0 && x < width && y >= 0 && y < height && channel >= 0 && channel < channels) {
        return data[(y * width + x) * channels + channel];
    }
    return 0;
}

glm::vec3 Image::getPixelRGB(int x, int y) const {
    if (channels >= 3) {
        return glm::vec3(
            getPixel(x, y, 0) / 255.0f,
            getPixel(x, y, 1) / 255.0f,
            getPixel(x, y, 2) / 255.0f
        );
    }
    unsigned char gray = getPixel(x, y, 0);
    float g = gray / 255.0f;
    return glm::vec3(g, g, g);
}

void Image::setPixelRGB(int x, int y, const glm::vec3& rgb) {
    if (channels >= 3) {
        setPixel(x, y, 0, static_cast<unsigned char>(glm::clamp(rgb.r, 0.0f, 1.0f) * 255));
        setPixel(x, y, 1, static_cast<unsigned char>(glm::clamp(rgb.g, 0.0f, 1.0f) * 255));
        setPixel(x, y, 2, static_cast<unsigned char>(glm::clamp(rgb.b, 0.0f, 1.0f) * 255));
    } else {
        float gray = (rgb.r + rgb.g + rgb.b) / 3.0f;
        setPixel(x, y, 0, static_cast<unsigned char>(glm::clamp(gray, 0.0f, 1.0f) * 255));
    }
}

Image Image::clone() const {
    Image img(width, height, channels);
    std::memcpy(img.data, data, width * height * channels);
    img.updateTexture();
    return img;
}

void Image::copyFrom(const Image& other) {
    if (width != other.width || height != other.height || channels != other.channels) {
        delete[] data;
        width = other.width;
        height = other.height;
        channels = other.channels;
        data = new unsigned char[width * height * channels];
    }
    std::memcpy(data, other.data, width * height * channels);
    updateTexture();
}

void Image::createTexture() {
    if (textureID) deleteTexture();
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Image::deleteTexture() {
    if (textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

void Image::updateTexture() {
    if (!data || !textureID) return;
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    GLenum format = GL_RGB;
    if (channels == 1) format = GL_RED;
    else if (channels == 4) format = GL_RGBA;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}