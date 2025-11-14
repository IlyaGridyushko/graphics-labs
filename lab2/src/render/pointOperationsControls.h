#pragma once
#include "../Image.h"
#include "../PointOperations.h"
#include "../../third_party/imgui/imgui.h"

inline void renderPointOperationsControls(Image& original, Image& result) {
    ImGui::Text("Point Operations + Linear Contrast");
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("Linear Contrast Enhancement", ImGuiTreeNodeFlags_DefaultOpen)) {
        static float minPercentile = 2.0f;
        static float maxPercentile = 98.0f;
        
        ImGui::Text("Automatic (Percentile-based):");
        ImGui::SliderFloat("Min Percentile", &minPercentile, 0.0f, 50.0f, "%.1f%%");
        ImGui::SliderFloat("Max Percentile", &maxPercentile, 50.0f, 100.0f, "%.1f%%");
        
        if (ImGui::Button("Apply Auto Contrast", ImVec2(-1, 0))) {
            result = PointOperations::linearContrast(original, minPercentile, maxPercentile);
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        static int minIn = 0, maxIn = 255;
        static int minOut = 0, maxOut = 255;
        
        ImGui::Text("Manual:");
        ImGui::SliderInt("Input Min", &minIn, 0, 255);
        ImGui::SliderInt("Input Max", &maxIn, 0, 255);
        ImGui::SliderInt("Output Min", &minOut, 0, 255);
        ImGui::SliderInt("Output Max", &maxOut, 0, 255);
        
        if (ImGui::Button("Apply Manual Contrast", ImVec2(-1, 0))) {
            result = PointOperations::linearContrastManual(original, minIn, maxIn, minOut, maxOut);
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Brightness & Contrast")) {
        static float brightness = 0.0f;
        static float contrast = 0.0f;
        
        ImGui::SliderFloat("Brightness", &brightness, -100.0f, 100.0f, "%.0f");
        ImGui::SliderFloat("Contrast", &contrast, -100.0f, 100.0f, "%.0f");
        
        if (ImGui::Button("Apply", ImVec2(-1, 0))) {
            result = PointOperations::adjustBrightnessContrast(original, brightness, contrast);
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            brightness = 0.0f;
            contrast = 0.0f;
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Gamma Correction")) {
        static float gamma = 1.0f;
        
        ImGui::SliderFloat("Gamma", &gamma, 0.1f, 5.0f, "%.2f");
        ImGui::TextWrapped("< 1.0: Brightens dark areas\n> 1.0: Darkens bright areas");
        
        if (ImGui::Button("Apply Gamma", ImVec2(-1, 0))) {
            result = PointOperations::gammaCorrection(original, gamma);
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Logarithmic Transform")) {
        static float logC = 1.0f;
        
        ImGui::SliderFloat("Constant C", &logC, 0.1f, 3.0f, "%.2f");
        ImGui::TextWrapped("Enhances dark regions, compresses bright regions");
        
        if (ImGui::Button("Apply Log Transform", ImVec2(-1, 0))) {
            result = PointOperations::logarithmicTransform(original, logC);
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Power Transform")) {
        static float power = 1.0f;
        static float powerC = 1.0f;
        
        ImGui::SliderFloat("Power", &power, 0.1f, 5.0f, "%.2f");
        ImGui::SliderFloat("Scale C", &powerC, 0.1f, 3.0f, "%.2f");
        
        if (ImGui::Button("Apply Power Transform", ImVec2(-1, 0))) {
            result = PointOperations::powerTransform(original, power, powerC);
        }
    }
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("Other Operations")) {
        if (ImGui::Button("Invert (Negative)", ImVec2(-1, 0))) {
            result = PointOperations::invert(original);
        }
        
        ImGui::Spacing();
        
        static int clipMin = 0, clipMax = 255;
        ImGui::Text("Brightness Clipping:");
        ImGui::SliderInt("Min Value", &clipMin, 0, 255);
        ImGui::SliderInt("Max Value", &clipMax, 0, 255);
        
        if (ImGui::Button("Apply Clipping", ImVec2(-1, 0))) {
            result = PointOperations::clipBrightness(original, clipMin, clipMax);
        }
        
        ImGui::Spacing();
        
        static int quantLevels = 8;
        ImGui::Text("Quantization:");
        ImGui::SliderInt("Levels", &quantLevels, 2, 64);
        
        if (ImGui::Button("Apply Quantization", ImVec2(-1, 0))) {
            result = PointOperations::quantize(original, quantLevels);
        }
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextWrapped("Point operations modify each pixel independently based on its value, without considering neighboring pixels.");
}   