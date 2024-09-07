#pragma once
#include <map>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "../Logger/Logger.h"
#include <string>
using namespace std;
class AssetManager{
private:
    std::map<std::string, SDL_Texture*> textures;
    // TODO: create a map for fonts
    // TODO: create a map for audio

public:
    AssetManager();
    ~AssetManager();

    void ClearAssets();
    void AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath);
    SDL_Texture* GetTexture(const std::string& assetId);
};