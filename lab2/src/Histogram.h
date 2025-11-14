#pragma once
#include "Image.h"
#include <vector>
#include <array>

class Histogram {
public:
    static std::array<int, 256> compute(const Image& img, int channel = -1);
    static std::array<int, 256> computeLuminance(const Image& img);

    static Image equalizeRGB(const Image& img);
    static Image equalizeHSV(const Image& img);

    static Image linearContrast(const Image& img, float minPercentile = 2.0f, float maxPercentile = 98.0f);
    static Image linearContrastManual(const Image& img, unsigned char minIn, unsigned char maxIn);

private:
    static glm::vec3 RGBtoHSV(const glm::vec3& rgb);
    static glm::vec3 HSVtoRGB(const glm::vec3& hsv);
};