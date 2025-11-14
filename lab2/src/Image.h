#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

class Image {
public:
    Image();
    Image(int width, int height, int channels = 3);
    ~Image();

    // Load/Save
    bool load(const std::string& filename);
    bool save(const std::string& filename) const;

    // Basic getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }
    uint8_t* getData() { return data.data(); }
    const uint8_t* getData() const { return data.data(); }
    
    // Pixel access
    uint8_t getPixel(int x, int y, int channel) const;
    void setPixel(int x, int y, int channel, uint8_t value);
    
    // Get a copy of the image
    Image clone() const;
    
    // Copy data from another image
    void copyFrom(const Image& other);
    
    // Create a grayscale version
    Image toGrayscale() const;
    
    // Check if image is valid
    bool isValid() const { return !data.empty() && width > 0 && height > 0; }

private:
    int width;
    int height;
    int channels;
    std::vector<uint8_t> data;
    
    // Helper to ensure coordinates are valid
    bool isValidCoord(int x, int y) const;
};
