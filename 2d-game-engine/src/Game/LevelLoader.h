#pragma once
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include <memory>
#include <SDL.h>
#include "Game.h"
#include <sol/sol.hpp>

class LevelLoader {
public:
	LevelLoader();
	~LevelLoader();
	void LoadLevel(sol::state& lua, int LevelNumber, SDL_Renderer* renderer,const unique_ptr<WorldCordinator>& worldCordinator, const unique_ptr<AssetManager>& assetManager);
};