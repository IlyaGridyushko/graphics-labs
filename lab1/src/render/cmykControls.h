#pragma once
#include "../Color.h"
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

#include "../../third_party/imgui/imgui.h"
#include "../../third_party/imgui/backends/imgui_impl_glfw.h"
#include "../../third_party/imgui/backends/imgui_impl_opengl3.h" 


void renderCMYKControls(Color& color) {
   
    bool valueChanged = false;
    
    glm::vec4 cmyk = color.getCMYK();
    
    ImGui::Text("Values [0;1]:");
    valueChanged |= ImGui::InputFloat("C", &cmyk.r, 0.01f, 0.1f, "%.3f");
    valueChanged |= ImGui::InputFloat("M", &cmyk.g, 0.01f, 0.1f, "%.3f");
    valueChanged |= ImGui::InputFloat("Y", &cmyk.b, 0.01f, 0.1f, "%.3f");
    valueChanged |= ImGui::InputFloat("K", &cmyk.a, 0.01f, 0.1f, "%.3f");
    
    ImGui::Text("Sliders:");
    int cmykPercent[4] = {
        static_cast<int>(cmyk.r * 100.0f),
        static_cast<int>(cmyk.g * 100.0f),
        static_cast<int>(cmyk.b * 100.0f),
        static_cast<int>(cmyk.a * 100.0f)
    };
    
    bool sliderChanged = false;
    sliderChanged |= ImGui::SliderInt("C##slider", &cmykPercent[0], 0, 100, "%d%%");
    if (sliderChanged && ImGui::IsItemActive()) {
        glm::vec4 updatedCmyk = color.getCMYK();
        updatedCmyk.r = cmykPercent[0] / 100.0f;
        color.setCMYK(updatedCmyk.r, updatedCmyk.g, updatedCmyk.b, updatedCmyk.a);
    }
    
    sliderChanged = false;
    sliderChanged |= ImGui::SliderInt("M##slider", &cmykPercent[1], 0, 100, "%d%%");
    if (sliderChanged && ImGui::IsItemActive()) {
        glm::vec4 updatedCmyk = color.getCMYK();
        updatedCmyk.g = cmykPercent[1] / 100.0f;
        color.setCMYK(updatedCmyk.r, updatedCmyk.g, updatedCmyk.b, updatedCmyk.a);
    }
    
    sliderChanged = false;
    sliderChanged |= ImGui::SliderInt("Y##slider", &cmykPercent[2], 0, 100, "%d%%");
    if (sliderChanged && ImGui::IsItemActive()) {
        glm::vec4 updatedCmyk = color.getCMYK();
        updatedCmyk.b = cmykPercent[2] / 100.0f;
        color.setCMYK(updatedCmyk.r, updatedCmyk.g, updatedCmyk.b, updatedCmyk.a);
    }
    
    sliderChanged = false;
    sliderChanged |= ImGui::SliderInt("K##slider", &cmykPercent[3], 0, 100, "%d%%");
    if (sliderChanged && ImGui::IsItemActive()) {
        glm::vec4 updatedCmyk = color.getCMYK();
        updatedCmyk.a = cmykPercent[3] / 100.0f;
        color.setCMYK(updatedCmyk.r, updatedCmyk.g, updatedCmyk.b, updatedCmyk.a);
    }
    
    if (valueChanged) {
        color.setCMYK(
            glm::clamp(cmyk.r, 0.0f, 1.0f),
            glm::clamp(cmyk.g, 0.0f, 1.0f),
            glm::clamp(cmyk.b, 0.0f, 1.0f),
            glm::clamp(cmyk.a, 0.0f, 1.0f)
        );
    }
}
