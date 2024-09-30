#pragma once
#include <string>
#include <SDL.h>
using namespace std;
struct SpriteComponent {
    string assetID;
    int width;
    int height;
    bool isFixed;
    unsigned int zIndex; 
    SDL_Rect srcRect;
    SDL_RendererFlip flip;

    SpriteComponent(string assetID = "", int width = 0, int height = 0, unsigned int zIndex = 0, int srcRectX = 0, int srcRectY = 0, bool isFixed = false) {
        this->assetID = assetID;
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->isFixed = isFixed;
        this->flip = SDL_FLIP_NONE;
        this->srcRect = { srcRectX, srcRectY, width, height };
    }
};