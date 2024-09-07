#include "./AssetManager.h"
#include "../Logger/Logger.h"
#include <SDL_image.h>

AssetManager::AssetManager() {
    Logger::Info("AssetManager constructor called!");
}

AssetManager::~AssetManager() {
    ClearAssets();
    Logger::Info("AssetManager destructor called!");
}

void AssetManager::ClearAssets() {
    for (auto texture: textures) {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Add the texture to the map
    textures.emplace(assetId, texture);

    Logger::Info("New texture added to the Asset Store with id = " + assetId);
}

SDL_Texture* AssetManager::GetTexture(const std::string& assetId) {
    return textures[assetId];
}
