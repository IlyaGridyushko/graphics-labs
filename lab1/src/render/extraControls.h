#pragma once
#include "../Color.h"
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

#include "../../third_party/imgui/imgui.h"
#include "../../third_party/imgui/backends/imgui_impl_glfw.h"
#include "../../third_party/imgui/backends/imgui_impl_opengl3.h" 


void renderExtraControls(Color& color,int windowWidth) {
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::BeginGroup();
    
    // Color Preview 
    ImGui::BeginGroup();
    ImGui::Text("Color Preview:");
    float previewWidth = windowWidth * 0.6f;
    float previewHeight = 240.0f;
    
    glm::vec3 rgb = color.getRGB();
    
    ImVec2 previewSize(previewWidth, previewHeight);
    ImGui::ColorButton("##preview", ImVec4(rgb.r, rgb.g, rgb.b, 1.0f), 
        ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, previewSize);

    ImGui::EndGroup();
    
    
    ImGui::SameLine(previewWidth + 30.0f); 

    // Color Picker
    ImGui::BeginGroup();
    ImGui::Text("Color Picker:");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

    ImGui::PushItemWidth(250.0f); 
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2)); 
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2)); 

    ImVec4 imColor = ImVec4(rgb.r, rgb.g, rgb.b, 1.0f);
    imColor.x = glm::clamp(imColor.x, 0.0f, 1.0f);
    imColor.y = glm::clamp(imColor.y, 0.0f, 1.0f);    
    imColor.z = glm::clamp(imColor.z, 0.0f, 1.0f);

    if (ImGui::ColorPicker3("##picker", (float*)&imColor, 
        ImGuiColorEditFlags_NoSidePreview | 
        ImGuiColorEditFlags_DisplayHex | 
        ImGuiColorEditFlags_NoInputs |
        ImGuiColorEditFlags_NoLabel |
        ImGuiColorEditFlags_PickerHueBar |
        ImGuiColorEditFlags_NoTooltip |
        ImGuiColorEditFlags_NoDragDrop |
        ImGuiColorEditFlags_NoBorder)) {
        
        color.setRGB(imColor.x, imColor.y, imColor.z);
    }

    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();

    ImGui::EndGroup();
    ImGui::EndGroup();
    ImGui::End();
}