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
    for (auto font: fonts) {
        TTF_CloseFont(font.second);
    }
    fonts.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    textures.emplace(assetId, texture);

    Logger::Info("New texture added to the Asset Store with id = " + assetId);
}

SDL_Texture* AssetManager::GetTexture(const std::string& assetId) {
    return textures[assetId];
}
vector<string> AssetManager::GetTextureNames() {
    vector<string> keys;
    for (std::map<string, SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it) {
        keys.push_back(it->first);
    }
    return keys;
}
void AssetManager::AddFont(const std::string& assetId, const std::string& filePath, int fontSize) {
    fonts.emplace(assetId, TTF_OpenFont(filePath.c_str(), fontSize));
    Logger::Info("New Font added to the Asset Store with id = " + assetId);

}

TTF_Font* AssetManager::GetFont(const std::string& assetId) {
    return fonts[assetId];
}
vector<string> AssetManager::GetFontNames() {
    vector<string> keys;
    for (std::map<string, TTF_Font*>::iterator it = fonts.begin(); it != fonts.end(); ++it) {
        keys.push_back(it->first);
    }
    return keys;
}
