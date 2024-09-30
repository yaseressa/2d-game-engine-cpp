#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
using namespace std;
using namespace glm;
struct TextLabelComponent {
    vec2 position;
    string text;
    string assetId;
    SDL_Color color;
    bool isFixed;
    TextLabelComponent(vec2 position = vec2(0), string text = "", string assetId = "", const SDL_Color& color = {0, 0, 0}, bool isFixed = true) : text(text), position(position), assetId(assetId), color(color), isFixed(isFixed)  {
    }
};