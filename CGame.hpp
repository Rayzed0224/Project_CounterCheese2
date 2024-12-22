#ifndef CGAME_HPP
#define CGAME_HPP

#include "CPlayer.hpp"
#include "Utils.hpp"
#include <vector>
#include <iostream>

class CGame {
public:
    uintptr_t baseClient;
    ViewMatrix viewMatrix;
    HANDLE hProcess;
    CGame() : baseClient(0), hProcess(nullptr), viewMatrix{} {}

    void init(const char* processName, const char* moduleName);
    void loop();
    void close();

private:
    std::vector<CPlayer> players; // Store players across frames
    void updatePlayers();
};

#endif