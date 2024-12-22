#include "../include/Renderer.h"
#include "../imgui/backends/imgui_impl_dx11.h"  // Ensure ImGui is properly set up
#include "../imgui/imgui.h"
#include <d3d11.h>  // DirectX 11 header
#include <windows.h>
#include <string>

extern ID3D11Device* g_pd3dDevice;          // DirectX device
extern ID3D11DeviceContext* g_pd3dContext; // DirectX device context

void Renderer::DrawBox(float x, float y, float width, float height, int color) {
    // Convert color to ImGui format
    ImU32 imguiColor = IM_COL32((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);

    // Use ImGui to draw the box
    ImGui::GetForegroundDrawList()->AddRect(
        ImVec2(x, y),                      // Top-left corner
        ImVec2(x + width, y + height),     // Bottom-right corner
        imguiColor,                        // Color
        0.0f,                              // Rounding
        ImDrawFlags_None,                  // Flags (e.g., rounded corners)
        2.0f                               // Thickness
    );
}

void Renderer::DrawLine(float x1, float y1, float x2, float y2, int color) {
    // Convert color to ImGui format
    ImU32 imguiColor = IM_COL32((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);

    // Use ImGui to draw the line
    ImGui::GetForegroundDrawList()->AddLine(
        ImVec2(x1, y1),
        ImVec2(x2, y2),
        imguiColor,
        2.0f // Line thickness
    );
}

// Helper function: Convert std::wstring to std::string (UTF-8)
std::string WStringToUTF8(const std::wstring& wideString) {
    if (wideString.empty()) return {};

    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8String(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &utf8String[0], sizeNeeded, nullptr, nullptr);
    return utf8String;
}

// Function implementation
void Renderer::RenderText(float x, float y, const wchar_t* text, int color) {
    // Convert wide text to UTF-8
    std::string utf8Text = WStringToUTF8(std::wstring(text));

    // Convert color to ImGui format
    ImU32 imguiColor = IM_COL32((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);

    // Use ImGui to draw the text
    ImGui::GetForegroundDrawList()->AddText(
        ImVec2(x, y),
        imguiColor,
        utf8Text.c_str()
    );
}
