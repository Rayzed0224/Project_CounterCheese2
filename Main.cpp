#include <dwmapi.h> // For transparency
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include "include/CGame.h"        // Ensure correct relative path
#include "include/ProcessManager.h"
#include "include/ImGuiManager.h"
#include "include/MemoryManager.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"

#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND CreateOverlayWindow(HINSTANCE hInstance, HWND gameWindow) {
    RECT gameRect;
    if (!GetWindowRect(gameWindow, &gameRect)) {
        std::wcerr << L"[ERROR] Failed to retrieve game window dimensions." << std::endl;
        return nullptr;
    }

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, hInstance, NULL, NULL, NULL, NULL, _T("Overlay"), NULL };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        wc.lpszClassName,
        _T("External Overlay"),
        WS_POPUP,
        gameRect.left,
        gameRect.top,
        gameRect.right - gameRect.left,
        gameRect.bottom - gameRect.top,
        NULL,
        NULL,
        wc.hInstance,
        NULL
    );

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    return hwnd;
}

// Continuously align the overlay in the game loop
void AlignOverlayWithGame(HWND overlayWindow, HWND gameWindow) {
    RECT gameRect;
    if (GetWindowRect(gameWindow, &gameRect)) {
        SetWindowPos(
            overlayWindow, HWND_TOPMOST,
            gameRect.left, gameRect.top,
            gameRect.right - gameRect.left,
            gameRect.bottom - gameRect.top,
            SWP_NOACTIVATE | SWP_NOSENDCHANGING
        );
    }
    else {
        std::wcerr << L"[ERROR] Failed to get game window dimensions." << std::endl;
    }
}

int main() {
    
    // Create the overlay window
    
    HWND gameWindow = FindWindow(nullptr, L"Counter-Strike 2");
    if (!gameWindow) {
        std::wcerr << L"[ERROR] Failed to find game window." << std::endl;
        return -1;
    }

    HWND overlayWindow = CreateOverlayWindow(GetModuleHandle(NULL), gameWindow);
    if (!overlayWindow) {
        std::wcerr << L"[ERROR] Failed to create overlay window." << std::endl;
        return -1;
    }

    // Attach to the game
        auto processStatus = ProcessMgr.Attach(L"cs2.exe");
        if (processStatus != StatusCode::SUCCEED) {
            std::wcerr << L"[ERROR] Failed to attach to the process 'cs2.exe'. StatusCode: " << static_cast<int>(processStatus) << std::endl;
            return -1;
        }

        // Create the game instance
        CGame game;

        // Create a MemoryManager instance
        MemoryManager memMgr;

        ImGuiManager imguiManager;

        // Step 2: Initialize ImGui for rendering UI
        if (!imguiManager.Initialize()) {
            std::wcerr << L"[ERROR] Failed to initialize ImGui." << std::endl;
            return -1;
        }

        // Step 3: Initialize addresses and memory
        if (!game.InitAddress(memMgr)) {
            std::wcerr << L"[ERROR] Failed to initialize game addresses." << std::endl;
            return -1;
        }

        // Step 4: Run the main game loop
        while (game.IsRunning()) {
            AlignOverlayWithGame(overlayWindow, gameWindow);

            // Run the game loop
            game.Run(memMgr, overlayWindow);
        }

        // Step 5: Cleanup resources
        imguiManager.Cleanup();

        std::wcout << L"[INFO] Exiting application." << std::endl;
        return 0;
}
