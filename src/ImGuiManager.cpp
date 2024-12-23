#include "../include/ImGuiManager.h"
#include "../imgui/imgui.h"                  // Core ImGui
#include "../imgui/backends/imgui_impl_dx11.h"        // ImGui DirectX11 backend
#include "../imgui/backends/imgui_impl_win32.h"       // ImGui Win32 backend
#include "../include/CGame.h" 
#include <Windows.h>
#include <iostream>
#include <d3d11.h>

// Global DirectX 11 variables
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dContext = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
IDXGISwapChain* swapChain = nullptr;


bool InitDirectX(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    if (FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, 1,
        D3D11_SDK_VERSION, &sd, &swapChain, &g_pd3dDevice, &featureLevel, &g_pd3dContext))) {
        std::wcerr << L"[ERROR] Failed to create DirectX 11 device and swap chain." << std::endl;
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
        std::wcerr << L"[ERROR] Failed to get back buffer from swap chain." << std::endl;
        return false;
    }
    if (FAILED(g_pd3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_mainRenderTargetView))) {
        std::wcerr << L"[ERROR] Failed to create render target view." << std::endl;
        backBuffer->Release();
        return false;
    }
    backBuffer->Release();

    return true;
}

bool ImGuiManager::Initialize() {
    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // Apply custom styles
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray background

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Get the game window handle
    HWND gameWindow = FindWindow(nullptr, L"Counter-Strike 2");
    if (!gameWindow) {
        std::wcerr << L"[ERROR] Failed to find game window." << std::endl;
        return false;
    }

    // Initialize DirectX 11
    if (!InitDirectX(gameWindow)) {
        std::wcerr << L"[ERROR] Failed to initialize DirectX 11." << std::endl;
        return false;
    }

    // Initialize ImGui backends
    if (!ImGui_ImplWin32_Init(gameWindow)) {
        std::wcerr << L"[ERROR] ImGui Win32 initialization failed." << std::endl;
        return false;
    }
    if (!ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dContext)) {
        std::wcerr << L"[ERROR] ImGui DirectX11 initialization failed." << std::endl;
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
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
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
    if (!drawData || drawData->TotalVtxCount == 0) {
        std::wcerr << L"[ERROR] ImGui draw data is empty or invalid!" << std::endl;
    }
    else {
        std::wcout << L"[DEBUG] ImGui draw data vertices: " << drawData->TotalVtxCount << std::endl;
    }

    ImGui_ImplDX11_RenderDrawData(drawData);
}


void ImGuiManager::Cleanup() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
    if (g_pd3dContext) { g_pd3dContext->Release(); g_pd3dContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }

    std::wcout << L"[INFO] ImGui cleaned up successfully." << std::endl;
}

