#pragma once
#include <SDL.h>
#include <memory> 
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
const int FPS =  1120;
const int FRAME_TARGET_TIME = 1000 / FPS;
class Game {
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool isRunning;
        bool isDebug = false;
		int msFromPreviousFrame = 0;
        unique_ptr<WorldCordinator> worldCordinator;
        unique_ptr<AssetManager> assetManager;
        unique_ptr<EventBus> eventBus;

    public:
        Game();
        ~Game();
        void Initialize();
        void Run();
        void Setup();
        void LoadLevel(int level);
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();
        int WindowWidth;
        int WindowHeight;
        
};
