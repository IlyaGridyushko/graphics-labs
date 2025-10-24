#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <string>

class Color{
public:
    Color(float r, float g, float b): rgb(r, g, b) {}
    Color(): rgb(0.0f, 0.0f, 0.0f), cmyk(0.0f, 0.0f, 0.0f, 1.0f), hsv(0.0f, 0.0f, 0.0f) {}

    void setRGB(float r, float g, float b) {
        rgb = glm::vec3(r, g, b);
        cmyk = rgbToCmyk(rgb);
        hsv = rgbToHsv(rgb);
    }
    void setCMYK(float c, float m, float y, float k) {
        cmyk = glm::vec4(c, m, y, k);
        rgb = cmykToRgb(cmyk);
        hsv = rgbToHsv(rgb);
    }
    void setHSV(float h, float s, float v) {
        hsv = glm::vec3(h, s, v);
        rgb = hsvToRgb(hsv);
        cmyk = rgbToCmyk(rgb);
    }

    glm::vec3 getRGB() const { return rgb;}
    glm::vec4 getCMYK() const { return cmyk;}
    glm::vec3 getHSV() const { return hsv;}
    std::string getHexRGB() const;

private:
    glm::vec3 rgb; // RGB in [0.0, 1.0]
    glm::vec4 cmyk; // CMYK in [0.0, 1.0]
    glm::vec3 hsv; // HSV in [0.0, 360.0], S and V in [0.0, 1.0]
    glm::vec4 rgbToCmyk(const glm::vec3& rgb);
    glm::vec3 cmykToRgb(const glm::vec4& cmyk);
    glm::vec3 rgbToHsv(const glm::vec3& rgb);
    glm::vec3 hsvToRgb(const glm::vec3& hsv);
};