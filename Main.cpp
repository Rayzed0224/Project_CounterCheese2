#include <iostream>
#include "include/CGame.h"        // Ensure correct relative path
#include "include/ProcessManager.h"
#include "include/ImGuiManager.h"
#include "include/MemoryManager.h"

int main() {
    // Create the game instance
    CGame game;

    // Create a MemoryManager instance
    MemoryManager memMgr;

    ImGuiManager imguiManager;

    // Step 1: Attach to the game process
    auto processStatus = ProcessMgr.Attach(L"cs2.exe"); // Attach to the CS2 process
    if (processStatus != StatusCode::SUCCEED) {
        std::wcerr << L"[ERROR] Failed to attach to the process 'cs2.exe'. StatusCode: "
            << static_cast<int>(processStatus) << std::endl;
        return -1;
    }
    std::wcout << L"[INFO] Successfully attached to 'cs2.exe' process." << std::endl;

    // Step 2: Initialize ImGui for rendering UI
    /*if (!imguiManager.Initialize()) {
        std::wcerr << L"[ERROR] Failed to initialize ImGui." << std::endl;
        return -1;
    }*/

    // Step 3: Initialize addresses and memory
    if (!game.InitAddress(memMgr)) {
        std::wcerr << L"[ERROR] Failed to initialize game addresses." << std::endl;
        return -1;
    }

    // Step 4: Run the main game loop
    game.Run(memMgr);

    // Step 5: Cleanup resources
    imguiManager.Cleanup();

    std::wcout << L"[INFO] Exiting application." << std::endl;
    return 0;
}
