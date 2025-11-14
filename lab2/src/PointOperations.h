#pragma once
#include "Image.h"

class PointOperations {
public:
    static Image linearContrast(const Image& img, float minPercentile = 2.0f, float maxPercentile = 98.0f);
    static Image linearContrastManual(const Image& img, unsigned char minIn, unsigned char maxIn,
                                      unsigned char minOut = 0, unsigned char maxOut = 255);

    static Image adjustBrightnessContrast(const Image& img, float brightness, float contrast);
    static Image gammaCorrection(const Image& img, float gamma);
    static Image logarithmicTransform(const Image& img, float c = 1.0f);
    static Image powerTransform(const Image& img, float power, float c = 1.0f);
    static Image invert(const Image& img);
    static Image clipBrightness(const Image& img, unsigned char minVal, unsigned char maxVal);
    static Image quantize(const Image& img, int levels);

    static Image bitwiseAND(const Image& img1, const Image& img2);
    static Image bitwiseOR(const Image& img1, const Image& img2);
    static Image bitwiseXOR(const Image& img1, const Image& img2);
    static Image bitwiseNOT(const Image& img);
    
private:
    static unsigned char applyGamma(unsigned char value, float gamma);
    static unsigned char applyLog(unsigned char value, float c);
};