#define GLM_ENABLE_EXPERIMENTAL
#include "Color.h"
#include <algorithm>
#include "Color.h"
#include <algorithm>
#include <glm/gtx/component_wise.hpp> 

glm::vec4 Color::rgbToCmyk(const glm::vec3& rgb) {
    float k = 1.0f - glm::compMax(rgb);
    if (k >= 1.0f - FLT_EPSILON) {
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    glm::vec3 cmy = (glm::vec3(1.0f) - rgb - glm::vec3(k)) / (1.0f - k);
    return glm::vec4(cmy, k);
}

glm::vec3 Color::cmykToRgb(const glm::vec4& cmyk) {
    glm::vec3 cmy = glm::vec3(cmyk);
    float k = cmyk.a;
    return (glm::vec3(1.0f) - cmy) * (1.0f - k);
}

glm::vec3 Color::rgbToHsv(const glm::vec3& rgb) {
    float maxComp = glm::compMax(rgb);
    float minComp = glm::compMin(rgb);
    float delta = maxComp - minComp;

    glm::vec3 hsv;
    hsv.b = maxComp;
    hsv.g = (maxComp > 0.0f) ? (delta / maxComp) : 0.0f;
    
    // Hue
    if (delta < FLT_EPSILON) {
        hsv.r = 0.0f;  // No color
    } else {
        if (maxComp == rgb.r) {
            hsv.r = 60.0f * glm::mod((rgb.g - rgb.b) / delta, 6.0f);
        } else if (maxComp == rgb.g) {
            hsv.r = 60.0f * ((rgb.b - rgb.r) / delta + 2.0f);
        } else { // maxComp == rgb.b
            hsv.r = 60.0f * ((rgb.r - rgb.g) / delta + 4.0f);
        }
    }

    hsv.r = glm::mod(hsv.r + 360.0f, 360.0f);
    return hsv;
}

glm::vec3 Color::hsvToRgb(const glm::vec3& hsv) {
    float h = hsv.r;
    float s = hsv.g;
    float v = hsv.b;
    
    float c = v * s;
    float x = c * (1.0f - glm::abs(glm::mod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    
    glm::vec3 rgb(0.0f);
    
    int sector = static_cast<int>(h / 60.0f);
    switch (sector % 6) {
        case 0: rgb = glm::vec3(c, x, 0.0f); break;
        case 1: rgb = glm::vec3(x, c, 0.0f); break;
        case 2: rgb = glm::vec3(0.0f, c, x); break;
        case 3: rgb = glm::vec3(0.0f, x, c); break;
        case 4: rgb = glm::vec3(x, 0.0f, c); break;
        case 5: rgb = glm::vec3(c, 0.0f, x); break;
    }
    
    return rgb + glm::vec3(m);
}

std::string Color::getHexRGB() const {
    int r = static_cast<int>(rgb.r * 255);
    int g = static_cast<int>(rgb.g * 255);
    int b = static_cast<int>(rgb.b * 255);
    char hex[8];
    snprintf(hex, sizeof(hex), "#%02X%02X%02X", r, g, b);
    return std::string(hex);
}