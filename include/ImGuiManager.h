#pragma once

#include <d3d11.h>
#include <Windows.h>
#include "CGame.h"

// Global DirectX 11 variables
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dContext;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
extern IDXGISwapChain* swapChain;

class ImGuiManager {
public:
    bool Initialize();   // Initialize ImGui
    void BeginFrame();   // Start a new ImGui frame
    void RenderUI(CGame& game);     // Render ImGui UI elements
    void EndFrame();     // End the ImGui frame and render
    void Cleanup();      // Cleanup ImGui resources
};
