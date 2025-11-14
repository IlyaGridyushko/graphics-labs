#pragma once
#include "../Image.h"
#include "../Histogram.h"
#include "../../third_party/imgui/imgui.h"
#include <array>

inline void renderHistogram(const std::array<int, 256>& hist, const char* label) {
    int maxVal = *std::max_element(hist.begin(), hist.end());
    if (maxVal == 0) maxVal = 1;
    
    ImGui::Text("%s", label);
    
    ImVec2 histSize(256, 100);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    draw_list->AddRectFilled(p, ImVec2(p.x + histSize.x, p.y + histSize.y), IM_COL32(40, 40, 40, 255));
    
    for (int i = 0; i < 256; i++) {
        float height = (hist[i] / (float)maxVal) * histSize.y;
        draw_list->AddLine(
            ImVec2(p.x + i, p.y + histSize.y),
            ImVec2(p.x + i, p.y + histSize.y - height),
            IM_COL32(255, 255, 255, 200)
        );
    }
    
    ImGui::Dummy(histSize);
}

inline void renderHistogramControls(Image& original, Image& result) {
    ImGui::Text("Histogram Processing");
    ImGui::Separator();
    
    static int method = 0;
    ImGui::RadioButton("Linear Contrast", &method, 0);
    ImGui::RadioButton("Histogram Equalization (RGB)", &method, 1);
    ImGui::RadioButton("Histogram Equalization (HSV)", &method, 2);
    
    if (method == 0) {
        static float minPercentile = 2.0f;
        static float maxPercentile = 98.0f;
        
        ImGui::SliderFloat("Min Percentile", &minPercentile, 0.0f, 50.0f, "%.1f%%");
        ImGui::SliderFloat("Max Percentile", &maxPercentile, 50.0f, 100.0f, "%.1f%%");
        
        if (ImGui::Button("Apply Linear Contrast")) {
            result = Histogram::linearContrast(original, minPercentile, maxPercentile);
        }
    } else if (method == 1) {
        if (ImGui::Button("Apply RGB Equalization")) {
            result = Histogram::equalizeRGB(original);
        }
        ImGui::TextWrapped("Equalizes each RGB channel separately");
    } else {
        if (ImGui::Button("Apply HSV Equalization")) {
            result = Histogram::equalizeHSV(original);
        }
        ImGui::TextWrapped("Equalizes only the Value channel in HSV space, preserving colors better");
    }
    
    ImGui::Spacing();
    ImGui::Separator();

    if (original.getData()) {
        auto histOrig = Histogram::computeLuminance(original);
        renderHistogram(histOrig, "Original Histogram");
    }
    
    if (result.getData()) {
        auto histResult = Histogram::computeLuminance(result);
        renderHistogram(histResult, "Result Histogram");
    }
}