#pragma once
#include "../Image.h"
#include "../../third_party/imgui/imgui.h"

inline void renderImageDisplay(const Image& img, const char* label, float maxWidth) {
    if (!img.getData()) return;
    
    ImGui::Text("%s (%dx%d)", label, img.getWidth(), img.getHeight());
    
    float aspectRatio = static_cast<float>(img.getHeight()) / img.getWidth();
    float displayWidth = std::min(maxWidth, static_cast<float>(img.getWidth()));
    float displayHeight = displayWidth * aspectRatio;
    
    ImGui::Image(
        (void*)(intptr_t)img.getTextureID(),
        ImVec2(displayWidth, displayHeight)
    );
}