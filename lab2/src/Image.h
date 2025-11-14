#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Image {
public:
    Image();
    Image(int width, int height, int channels);
    ~Image();

    Image(const Image& other);
    Image& operator=(const Image& other);
    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

    bool load(const std::string& filepath);
    bool save(const std::string& filepath);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }
    unsigned char* getData() { return data; }
    const unsigned char* getData() const { return data; }
    
    void setPixel(int x, int y, int channel, unsigned char value);
    unsigned char getPixel(int x, int y, int channel) const;
    
    glm::vec3 getPixelRGB(int x, int y) const;
    void setPixelRGB(int x, int y, const glm::vec3& rgb);
    
    Image clone() const;
    void copyFrom(const Image& other);
    
    unsigned int getTextureID() const { return textureID; }
    void updateTexture();

private:
    int width;
    int height;
    int channels;
    unsigned char* data;
    unsigned int textureID;
    
    void createTexture();
    void deleteTexture();
};