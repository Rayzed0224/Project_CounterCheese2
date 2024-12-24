#include "../include/ImGuiManager.h"
#include "../imgui/imgui.h"                  // Core ImGui
#include "../imgui/backends/imgui_impl_dx11.h"        // ImGui DirectX11 backend
#include "../imgui/backends/imgui_impl_win32.h"       // ImGui Win32 backend
#include "../include/CGame.h" 
#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <dxgi.h>

// Global DirectX 11 variables
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dContext = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
IDXGISwapChain* swapChain = nullptr;


bool ImGuiManager::InitDirectX(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &sd, &swapChain, &g_pd3dDevice, nullptr, &g_pd3dContext);

    if (FAILED(hr)) {
        std::wcerr << L"[ERROR] Failed to create DirectX device and swap chain. HRESULT: " << std::hex << hr << std::endl;
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr)) {
        std::wcerr << L"[ERROR] Failed to get back buffer. HRESULT: " << std::hex << hr << std::endl;
        return false;
    }

    hr = g_pd3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_mainRenderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) {
        std::wcerr << L"[ERROR] Failed to create render target view. HRESULT: " << std::hex << hr << std::endl;
        return false;
    }

    RECT rect;
    if (GetClientRect(hwnd, &rect)) {
        std::wcout << L"[DEBUG] Window dimensions: "
            << (rect.right - rect.left) << "x" << (rect.bottom - rect.top) << std::endl;
    }
    else {
        std::wcerr << L"[ERROR] GetClientRect failed. Error code: " << GetLastError() << std::endl;
        rect.right = rect.left + 1920; // Default fallback
        rect.bottom = rect.top + 1080;
    }

    FLOAT width = static_cast<FLOAT>(rect.right - rect.left);
    FLOAT height = static_cast<FLOAT>(rect.bottom - rect.top);

    if (width <= 0 || height <= 0) {
        std::wcerr << L"[ERROR] Invalid viewport dimensions. Setting default." << std::endl;
        width = 1920.0f;
        height = 1080.0f;
    }

    D3D11_VIEWPORT viewport = {};
    viewport.Width = (float)GetSystemMetrics(SM_CXSCREEN); // Full screen width
    viewport.Height = (float)GetSystemMetrics(SM_CYSCREEN); // Full screen height
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    g_pd3dContext->RSSetViewports(1, &viewport);

    std::wcout << L"[DEBUG] DirectX initialized with viewport: " << width << "x" << height << std::endl;
    return true;
}

bool ImGuiManager::Initialize(HWND overlayWindow) {
    // Validate overlay window
    if (!overlayWindow) {
        std::wcerr << L"[ERROR] Overlay window handle is invalid." << std::endl;
        return false;
    }

    // Initialize DirectX
    if (!InitDirectX(overlayWindow)) {
        std::wcerr << L"[ERROR] Failed to initialize DirectX." << std::endl;
        return false;
    }
    std::wcout << L"[DEBUG] DirectX initialized successfully." << std::endl;

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    std::wcout << L"[DEBUG] ImGui context created." << std::endl;

    // Initialize ImGui backends
    if (!ImGui_ImplWin32_Init(overlayWindow) || !ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext)) {
        std::wcerr << L"[ERROR] Failed to initialize ImGui backends." << std::endl;
        return false;
    }
    std::wcout << L"[INFO] ImGui initialized successfully." << std::endl;

    return true;
}

void ImGuiManager::BeginFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::RenderUI(CGame& game) {
    
    ImGui::Begin("Debug Menu");
    ImGui::Text("Test Window - If you can see this, ImGui is working.");

    // Toggle ESP rendering
    /*static bool enableESP = true;
    if (ImGui::Checkbox("Enable ESP", &enableESP)) {
        game.SetESPState(enableESP); // Call the method to update ESP state
        std::wcout << L"[INFO] ESP is Enabled" << std::endl;
    }*/

    ImGui::End();
}

void ImGuiManager::EndFrame() {
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    if (drawData && drawData->TotalVtxCount > 0) {
        std::wcout << L"[DEBUG] ImGui draw data vertices: " << drawData->TotalVtxCount << std::endl;
    }
    else {
        std::wcout << L"[DEBUG] No ImGui draw data generated." << std::endl;
    }
    ImGui_ImplDX11_RenderDrawData(drawData);
}

void ImGuiManager::Cleanup() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (g_mainRenderTargetView) g_mainRenderTargetView->Release();
    if (swapChain) swapChain->Release();
    if (g_pd3dContext) g_pd3dContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();

    std::wcout << L"[INFO] ImGui cleaned up successfully." << std::endl;
}

