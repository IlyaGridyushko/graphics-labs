#pragma once
#include "Image.h"
#include <array>

class ThresholdProcessing {
public:
    static Image otsuThreshold(const Image& img);
    static unsigned char calculateOtsuThreshold(const Image& img);

    static Image triangleThreshold(const Image& img);
    static unsigned char calculateTriangleThreshold(const Image& img);

    static Image fixedThreshold(const Image& img, unsigned char threshold);
    static Image doubleThreshold(const Image& img, unsigned char lowThreshold, unsigned char highThreshold);
    static std::array<int, 256> computeHistogram(const Image& img);

private:
    static float calculateImageIntensity(const Image& img);
};