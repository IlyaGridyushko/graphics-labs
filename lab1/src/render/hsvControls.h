#pragma once
#include "../Color.h"
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

#include "../../third_party/imgui/imgui.h"
#include "../../third_party/imgui/backends/imgui_impl_glfw.h"
#include "../../third_party/imgui/backends/imgui_impl_opengl3.h" 


void renderHSVControls(Color& color) {
 
    bool valueChanged = false;
    
    glm::vec3 hsv = color.getHSV();
    
    
    ImGui::Text("Values H:[0;360], S,V:[0;1]:");
    valueChanged |= ImGui::InputFloat("H", &hsv.r, 1.0f, 10.0f, "%.1f");
    valueChanged |= ImGui::InputFloat("S", &hsv.g, 0.01f, 0.1f, "%.3f");
    valueChanged |= ImGui::InputFloat("V", &hsv.b, 0.01f, 0.1f, "%.3f");
    
    
    ImGui::Text("Sliders:");
    
    bool sliderChanged = false;
    sliderChanged |= ImGui::SliderFloat("H##slider", &hsv.r, 0, 360, "%.1f°");
    if (sliderChanged && ImGui::IsItemActive()) {
        hsv.r = fmod(hsv.r + 360.0f, 360.0f); // Ensure proper range
        glm::vec3 updatedHsv = color.getHSV();
        updatedHsv.r = hsv.r;
        color.setHSV(updatedHsv.r, updatedHsv.g, updatedHsv.b);
    }
    
    sliderChanged = false;
    sliderChanged |= ImGui::SliderFloat("S##slider", &hsv.g, 0, 1, "%.2f");
    if (sliderChanged && ImGui::IsItemActive()) {
        glm::vec3 updatedHsv = color.getHSV();
        updatedHsv.g = glm::clamp(hsv.g, 0.0f, 1.0f);
        color.setHSV(updatedHsv.r, updatedHsv.g, updatedHsv.b);
    }
    
    sliderChanged = false;
    sliderChanged |= ImGui::SliderFloat("V##slider", &hsv.b, 0, 1, "%.2f");
    if (sliderChanged && ImGui::IsItemActive()) {
        glm::vec3 updatedHsv = color.getHSV();
        updatedHsv.b = glm::clamp(hsv.b, 0.0f, 1.0f);
        color.setHSV(updatedHsv.r, updatedHsv.g, updatedHsv.b);
    }
    
   
    ImGui::Text("HSV Color Wheel:");
    ImVec4 hsvColor = ImVec4(color.getRGB().r, color.getRGB().g, color.getRGB().b, 1.0f);
    if (ImGui::ColorPicker3("##HSVWheel", (float*)&hsvColor,
                          ImGuiColorEditFlags_PickerHueWheel |
                          ImGuiColorEditFlags_NoSidePreview |
                          ImGuiColorEditFlags_DisplayHSV |
                          ImGuiColorEditFlags_NoInputs)) {
     
        color.setRGB(hsvColor.x, hsvColor.y, hsvColor.z);
        return; 
    }
    
    if (valueChanged) {
        while (hsv.r > 360.0f) hsv.r -= 360.0f;
        while (hsv.r < 0.0f) hsv.r += 360.0f;
        
        hsv.g = glm::clamp(hsv.g, 0.0f, 1.0f); 
        hsv.b = glm::clamp(hsv.b, 0.0f, 1.0f);
        
        color.setHSV(hsv.r, hsv.g, hsv.b);
    }
}