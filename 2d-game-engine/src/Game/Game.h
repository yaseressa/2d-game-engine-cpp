#pragma once
#include <SDL.h>
#include <memory> 
#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"
#include "../EventBus/EventBus.h"
#include <sol/sol.hpp>
//using namespace sol;
const int FPS =  1120;
const int FRAME_TARGET_TIME = 1000 / FPS;
class Game {
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Rect camera;
        bool isRunning;
        bool isDebug = false;
		int msFromPreviousFrame = 0;
        unique_ptr<AssetManager> assetManager;
        unique_ptr<EventBus> eventBus;
        unique_ptr<WorldCordinator> worldCordinator;
        sol::state lua;


    public:
        Game();
        ~Game();
        void Initialize();
        void Run();
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();
        static int WindowWidth;
        static int WindowHeight;
        static int MapWidth;
        static int MapHeight;

        
};
