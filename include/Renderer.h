#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "Entity.h"

class Renderer {
public:
    static void DrawBox(float x, float y, float width, float height, int color);
    static void DrawLine(float x1, float y1, float x2, float y2, int color);
    static void RenderText(float x, float y, const wchar_t* text, int color); // Renamed
    static void RenderESP(const std::vector<Entity>& entities, const float* viewMatrix, int screenWidth, int screenHeight);
};