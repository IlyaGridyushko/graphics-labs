#pragma once
#include "../Color.h"
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

#include "../../third_party/imgui/imgui.h"
#include "../../third_party/imgui/backends/imgui_impl_glfw.h"
#include "../../third_party/imgui/backends/imgui_impl_opengl3.h"

#include <cstdio>
#include <cstring>
#include <cmath>

void renderRGBControls(Color &color) {
    bool changed = false;

    glm::vec3 rgb = color.getRGB();

    // Values [0;1]
    ImGui::Text("Values [0;1]:");
    float r = rgb.r, g = rgb.g, b = rgb.b;
    if (ImGui::InputFloat("R", &r, 0.01f, 0.1f, "%.3f")) changed = true;
    if (ImGui::InputFloat("G", &g, 0.01f, 0.1f, "%.3f")) changed = true;
    if (ImGui::InputFloat("B", &b, 0.01f, 0.1f, "%.3f")) changed = true;
    if (changed) {
        rgb = glm::vec3(r, g, b);
    }

    // Values [0;255]
    ImGui::Text("Values [0;255]:");
    int rgb255[3] = {
        static_cast<int>(std::round(rgb.r * 255.0f)),
        static_cast<int>(std::round(rgb.g * 255.0f)),
        static_cast<int>(std::round(rgb.b * 255.0f))
    };

    bool intsChanged = false;
    intsChanged |= ImGui::InputInt("R ", &rgb255[0]);
    intsChanged |= ImGui::InputInt("G ", &rgb255[1]);
    intsChanged |= ImGui::InputInt("B ", &rgb255[2]);

    if (intsChanged) {
        rgb255[0] = glm::clamp(rgb255[0], 0, 255);
        rgb255[1] = glm::clamp(rgb255[1], 0, 255);
        rgb255[2] = glm::clamp(rgb255[2], 0, 255);
        rgb = glm::vec3(
            rgb255[0] / 255.0f,
            rgb255[1] / 255.0f,
            rgb255[2] / 255.0f
        );
        changed = true;
    }

    ImGui::Text("Sliders:");
    bool sR = ImGui::SliderInt("R##slider", &rgb255[0], 0, 255);
    bool sG = ImGui::SliderInt("G##slider", &rgb255[1], 0, 255);
    bool sB = ImGui::SliderInt("B##slider", &rgb255[2], 0, 255);
    if (sR || sG || sB) {
        rgb255[0] = glm::clamp(rgb255[0], 0, 255);
        rgb255[1] = glm::clamp(rgb255[1], 0, 255);
        rgb255[2] = glm::clamp(rgb255[2], 0, 255);
        rgb = glm::vec3(
            rgb255[0] / 255.0f,
            rgb255[1] / 255.0f,
            rgb255[2] / 255.0f
        );
        changed = true;
    }

   
    char hexBuf[7] = "000000"; 
    bool hexEditing = false;

    if (!hexEditing) {
        int rr = static_cast<int>(glm::clamp(int(std::round(rgb.r * 255.0f)), 0, 255));
        int gg = static_cast<int>(glm::clamp(int(std::round(rgb.g * 255.0f)), 0, 255));
        int bb = static_cast<int>(glm::clamp(int(std::round(rgb.b * 255.0f)), 0, 255));
        std::snprintf(hexBuf, sizeof(hexBuf), "%02X%02X%02X", rr, gg, bb);
    }

    ImGui::Text("HEX Color:");
    if (ImGui::InputText("HEX", hexBuf, IM_ARRAYSIZE(hexBuf),
            ImGuiInputTextFlags_CharsUppercase | 
            ImGuiInputTextFlags_CharsHexadecimal)) {
        hexEditing = true;

        size_t len = std::strlen(hexBuf);
        if (len == 6 || len == 3) {
            char expanded[7] = {0};
            if (len == 3) {
                // Expand #RGB -> #RRGGBB
                expanded[0] = hexBuf[0]; expanded[1] = hexBuf[0];
                expanded[2] = hexBuf[1]; expanded[3] = hexBuf[1];
                expanded[4] = hexBuf[2]; expanded[5] = hexBuf[2];
            } else {
                std::memcpy(expanded, hexBuf, 6);
            }

            unsigned value = static_cast<unsigned>(std::strtoul(expanded, nullptr, 16));
            unsigned rr = (value >> 16) & 0xFF;
            unsigned gg = (value >> 8) & 0xFF;
            unsigned bb = (value) & 0xFF;

            rgb = glm::vec3(rr / 255.0f, gg / 255.0f, bb / 255.0f);
            changed = true;
        }
    }
    bool hexActive = ImGui::IsItemActive();

    if (!hexActive && hexEditing) {
        hexEditing = false;
        int rr = static_cast<int>(glm::clamp(int(std::round(rgb.r * 255.0f)), 0, 255));
        int gg = static_cast<int>(glm::clamp(int(std::round(rgb.g * 255.0f)), 0, 255));
        int bb = static_cast<int>(glm::clamp(int(std::round(rgb.b * 255.0f)), 0, 255));
        std::snprintf(hexBuf, sizeof(hexBuf), "%02X%02X%02X", rr, gg, bb);
    }

    if (changed) {
        color.setRGB(
            glm::clamp(rgb.r, 0.0f, 1.0f),
            glm::clamp(rgb.g, 0.0f, 1.0f),
            glm::clamp(rgb.b, 0.0f, 1.0f)
        );
    }
}