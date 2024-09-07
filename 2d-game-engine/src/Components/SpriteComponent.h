#pragma once
#include <string>
#include <SDL.h>
using namespace std;
struct SpriteComponent {
    string assetID;
    int width;
    int height;
    unsigned int zIndex; 
    SDL_Rect srcRect;

    SpriteComponent(string assetID = "", int width = 0, int height = 0, int srcRectX = 0, int srcRectY = 0, unsigned int zIndex = 0) {
        this->assetID = assetID;
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->srcRect = { srcRectX, srcRectY, width, height };
    }
};