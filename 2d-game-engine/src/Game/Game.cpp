#include "Game.h"
#include <iostream>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include "../Logger/Logger.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxCollidorComponent.h"
#include "../System/MovementSystem.h"
#include "../System/RenderSystem.h"
#include "../System/AnimationSystem.h"
#include "../System/CollisionSystem.h"
#include "../System/CollidorRectangleRenderSystem.h"
#include <fstream>
using namespace std;
using namespace glm;
Game::Game() {
    isRunning = false;
	worldCordinator = make_unique<WorldCordinator>();
	assetManager = make_unique<AssetManager>();
}

Game::~Game() {

}

void Game::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		cerr << "Error initializing SDL";
		return;
	}
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	WindowWidth = /*displayMode.w*/ 1000;
	WindowHeight = /*displayMode.h*/ 600;
	window = SDL_CreateWindow("Steriods", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, SDL_WINDOW_BORDERLESS);
	if (!window) {
		Logger::Error("Error creating SDL window.");
		return;

	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		Logger::Error("Error creating SDL renderer.");
	}
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	this -> isRunning = true;
}

void Game::Run() {
	Setup();
	Logger::Info("Game is running");
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}
void Game::LoadLevel(int level) {
	worldCordinator->AddSystem<MovementSystem>();
	worldCordinator->AddSystem<RenderSystem>();
	worldCordinator->AddSystem<AnimationSystem>();
	worldCordinator->AddSystem<CollisionSystem>();
	worldCordinator->AddSystem<CollidorRectangleRenderSystem>();

	assetManager->AddTexture(renderer, "tank", "./assets/images/tank-panther-up.png");
	assetManager->AddTexture(renderer, "truck", "./assets/images/truck-ford-right.png");
	assetManager->AddTexture(renderer, "chopper", "./assets/images/chopper.png");
	assetManager->AddTexture(renderer, "radar", "./assets/images/radar.png");
	assetManager->AddTexture(renderer, "tile", "./assets/tilemaps/jungle.png");


	int tileSize = 32, mapNumCols = 25, mapNumRows = 20;
	fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");
	double tileScale = 2.0;

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = atoi(&ch) * tileSize;
			mapFile.get(ch);
			Entity tile = worldCordinator->CreateEntity();
			tile.AddComponent<TransformComponent>(vec2(x * tileSize * tileScale, y * tileSize * tileScale), vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tile", tileSize, tileSize, srcRectX, srcRectY);
		}
	}
	mapFile.close();
	Entity tank = worldCordinator->CreateEntity();
	tank.AddComponent<TransformComponent>(vec2(10.0, 10.0), vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(vec2(50.0, .0));
	tank.AddComponent<SpriteComponent>("tank", 100, 100, NULL, NULL, 1);
	tank.AddComponent<BoxCollidorComponent>(100, 100);
	
	Entity truck = worldCordinator->CreateEntity();
	truck.AddComponent<TransformComponent>(vec2(1000.0, 10.0), vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(vec2(-10.0, .0));
	truck.AddComponent<SpriteComponent>("truck", 100, 100, NULL, NULL, 2);
	truck.AddComponent<BoxCollidorComponent>(100, 100);
	
	Entity chopper = worldCordinator->CreateEntity();
	chopper.AddComponent<TransformComponent>(vec2(10.0, 10.0), vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(vec2(.0, .0));
	chopper.AddComponent<SpriteComponent>("chopper", 32, 32, NULL, NULL, 1);
	chopper.AddComponent<AnimationComponent>(2, 5, true);

	Entity radar = worldCordinator->CreateEntity();
	radar.AddComponent<TransformComponent>(vec2(WindowWidth - 74, 10), vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(vec2(.0, .0));
	radar.AddComponent<SpriteComponent>("radar", 64, 64, NULL, NULL, 2);
	radar.AddComponent<AnimationComponent>(8, 5, true);

}

void Game::Setup() {

	LoadLevel(1);
	
}


void Game::ProcessInput() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		int key = sdlEvent.key.keysym.sym;
		switch (sdlEvent.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (key == SDLK_ESCAPE) {
				isRunning = false;
			}
			if (key == SDLK_d) {
				isDebug = !isDebug;
			}
			break;
		}
	}
}


void Game::Update() {
	int wait = FRAME_TARGET_TIME - (SDL_GetTicks() - msFromPreviousFrame);
	if (wait > 0 and wait <= FRAME_TARGET_TIME) {
		SDL_Delay(wait);
	}
	float deltaTime = (SDL_GetTicks() - msFromPreviousFrame) / 1000.0F;
	msFromPreviousFrame = SDL_GetTicks();

	worldCordinator->Update();
	worldCordinator->GetSystem<MovementSystem>().Update(deltaTime);
	worldCordinator->GetSystem<AnimationSystem>().Update();
	worldCordinator->GetSystem<CollisionSystem>().Update(renderer);
}


void Game::Render() {
	worldCordinator->GetSystem<RenderSystem>().Update(renderer, assetManager);
	if (isDebug)
		worldCordinator->GetSystem<CollidorRectangleRenderSystem>().Update(renderer);

	SDL_RenderPresent(renderer);

}
void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}