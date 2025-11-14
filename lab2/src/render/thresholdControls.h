#pragma once
#include "../Image.h"
#include "../ThresholdProcessing.h"
#include "../../third_party/imgui/imgui.h"
#include <array>

inline void renderThresholdHistogram(const std::array<int, 256>& hist, unsigned char threshold, const char* label) {
    int maxVal = *std::max_element(hist.begin(), hist.end());
    if (maxVal == 0) maxVal = 1;
    
    ImGui::Text("%s (Threshold: %d)", label, threshold);
    
    ImVec2 histSize(256, 100);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    draw_list->AddRectFilled(p, ImVec2(p.x + histSize.x, p.y + histSize.y), IM_COL32(40, 40, 40, 255));

    for (int i = 0; i < 256; i++) {
        float height = (hist[i] / (float)maxVal) * histSize.y;
        ImU32 color = (i < threshold) ? IM_COL32(100, 100, 255, 200) : IM_COL32(255, 100, 100, 200);
        draw_list->AddLine(
            ImVec2(p.x + i, p.y + histSize.y),
            ImVec2(p.x + i, p.y + histSize.y - height),
            color
        );
    }

    draw_list->AddLine(
        ImVec2(p.x + threshold, p.y),
        ImVec2(p.x + threshold, p.y + histSize.y),
        IM_COL32(0, 255, 0, 255),
        2.0f
    );
    
    ImGui::Dummy(histSize);
}

inline void renderThresholdControls(Image& original, Image& result) {
    ImGui::Text("Global Threshold Processing");
    ImGui::Separator();
    
    static int method = 0;
    ImGui::Text("Select Method:");
    ImGui::RadioButton("Otsu Method (Automatic)", &method, 0);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Automatically finds optimal threshold by maximizing inter-class variance");
    }
    
    ImGui::RadioButton("Triangle Method", &method, 1);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Good for images with one dominant peak in histogram");
    }
    
    ImGui::RadioButton("Fixed Threshold", &method, 2);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Manual threshold selection");
    }
    
    ImGui::RadioButton("Double Threshold", &method, 3);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Used for edge detection (strong/weak edges)");
    }
    
    ImGui::Spacing();
    
    static int fixedThreshold = 127;
    static int lowThreshold = 50;
    static int highThreshold = 150;
    
    if (method == 0) {
        if (ImGui::Button("Apply Otsu Method", ImVec2(-1, 0))) {
            result = ThresholdProcessing::otsuThreshold(original);
        }
        
        if (original.getData()) {
            unsigned char otsuThresh = ThresholdProcessing::calculateOtsuThreshold(original);
            ImGui::Text("Calculated threshold: %d", otsuThresh);
        }
        
    } else if (method == 1) {
        if (ImGui::Button("Apply Triangle Method", ImVec2(-1, 0))) {
            result = ThresholdProcessing::triangleThreshold(original);
        }
        
        if (original.getData()) {
            unsigned char triThresh = ThresholdProcessing::calculateTriangleThreshold(original);
            ImGui::Text("Calculated threshold: %d", triThresh);
        }
        
    } else if (method == 2) {
        ImGui::SliderInt("Threshold", &fixedThreshold, 0, 255);
        
        if (ImGui::Button("Apply Fixed Threshold", ImVec2(-1, 0))) {
            result = ThresholdProcessing::fixedThreshold(original, fixedThreshold);
        }
        
    } else if (method == 3) {
        ImGui::SliderInt("Low Threshold", &lowThreshold, 0, 255);
        ImGui::SliderInt("High Threshold", &highThreshold, 0, 255);
        
        if (lowThreshold > highThreshold) {
            lowThreshold = highThreshold;
        }
        
        if (ImGui::Button("Apply Double Threshold", ImVec2(-1, 0))) {
            result = ThresholdProcessing::doubleThreshold(original, lowThreshold, highThreshold);
        }
        
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "White: Strong edges");
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), "Gray: Weak edges");
        ImGui::TextColored(ImVec4(0, 0, 0, 1), "Black: Non-edges");
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (original.getData()) {
        auto hist = ThresholdProcessing::computeHistogram(original);
        unsigned char thresh = 127;
        
        if (method == 0) {
            thresh = ThresholdProcessing::calculateOtsuThreshold(original);
        } else if (method == 1) {
            thresh = ThresholdProcessing::calculateTriangleThreshold(original);
        } else if (method == 2) {
            thresh = fixedThreshold;
        }
        
        if (method != 3) {
            renderThresholdHistogram(hist, thresh, "Histogram with Threshold");
        }
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextWrapped("Description:");
    ImGui::TextWrapped("Global thresholding converts grayscale images to binary by comparing each pixel to a threshold value. Pixels above threshold become white, below become black.");
}