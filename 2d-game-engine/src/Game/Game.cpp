#include "LevelLoader.h"
#include "Game.h"
#include <iostream>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <glm/glm.hpp>
#include "../Logger/Logger.h"
#include "../System/MovementSystem.h"
#include "../System/RenderSystem.h"
#include "../System/AnimationSystem.h"
#include "../System/CollisionSystem.h"
#include "../System/DamageSystem.h"
#include "../System/KeyboardMovementSystem.h"
#include "../System/CollidorRectangleRenderSystem.h"
#include "../System/CameraFollowSystem.h"
#include "../System/ProjectileEmitSystem.h"
#include "../System/CycleSystem.h"
#include "../System/RenderTextSystem.h"
#include "../System/RenderHealthSystem.h"
#include "../System/RenderGuiSystem.h"
#include "../System/ScriptSystem.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include <fstream>
using namespace std;
using namespace glm;
int Game::WindowHeight;
int Game::WindowWidth;
int Game:: MapHeight;
int Game:: MapWidth;
Game::Game() {
    isRunning = false;
	worldCordinator = make_unique<WorldCordinator>();
	assetManager = make_unique<AssetManager>();
	eventBus = make_unique<EventBus>();
}

Game::~Game() {

}

void Game::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		Logger::Error("Error initializing SDL");
		return;
	}
	if (TTF_Init() != 0) {
		Logger::Error("Error initializing SDL TTF.");
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
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, WindowWidth, WindowHeight);
	camera = { 0, 0, WindowWidth, WindowHeight };
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
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

void Game::Setup() {
	worldCordinator->AddSystem<MovementSystem>();
	worldCordinator->AddSystem<RenderSystem>();
	worldCordinator->AddSystem<AnimationSystem>();
	worldCordinator->AddSystem<CollisionSystem>();
	worldCordinator->AddSystem<DamageSystem>();
	worldCordinator->AddSystem<KeyboardMovementSystem>();
	worldCordinator->AddSystem<CollidorRectangleRenderSystem>();
	worldCordinator->AddSystem<CameraFollowSystem>();
	worldCordinator->AddSystem<ProjectileEmitSystem>();
	worldCordinator->AddSystem<ProjectileLifecycleSystem>();
	worldCordinator->AddSystem<RenderTextSystem>();
	worldCordinator->AddSystem<RenderHealthSystem>();
	worldCordinator->AddSystem<RenderGuiSystem>();
	worldCordinator->AddSystem<ScriptSystem>();
	
	worldCordinator->GetSystem<ScriptSystem>().CreateLuaBindings(lua);
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	LevelLoader loader;
	loader.LoadLevel(lua, 2, renderer, worldCordinator, assetManager);
	
}


void Game::ProcessInput() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		ImGuiIO& io = ImGui::GetIO();
		int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		
		switch (sdlEvent.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
			if (sdlEvent.key.keysym.sym == SDLK_F1) {
				isDebug = !isDebug;
			}
			eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
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
	eventBus->Reset();
	worldCordinator->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	worldCordinator->GetSystem<KeyboardMovementSystem>().SubscribeToEvents(eventBus);
	worldCordinator->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
	worldCordinator->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
	worldCordinator->Update();
	worldCordinator->GetSystem<MovementSystem>().Update(deltaTime);
	worldCordinator->GetSystem<AnimationSystem>().Update();
	worldCordinator->GetSystem<CollisionSystem>().Update(renderer, eventBus);
	worldCordinator->GetSystem<ProjectileEmitSystem>().Update(worldCordinator);
	worldCordinator->GetSystem<CameraFollowSystem>().Update(camera);
	worldCordinator->GetSystem<ProjectileLifecycleSystem>().Update();
	worldCordinator->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());
}


void Game::Render() {
	worldCordinator->GetSystem<RenderSystem>().Update(renderer, assetManager, camera);
	worldCordinator->GetSystem<RenderTextSystem>().Update(renderer, assetManager, camera);
	worldCordinator->GetSystem<RenderHealthSystem>().Update(renderer, assetManager, camera);
	if (isDebug) {
		worldCordinator->GetSystem<CollidorRectangleRenderSystem>().Update(renderer, camera);
		worldCordinator->GetSystem<RenderGuiSystem>().Update(worldCordinator, assetManager, camera);
		
	}
	SDL_RenderPresent(renderer);

}
void Game::Destroy() {
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}