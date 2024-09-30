#pragma once
#include "../ECS/ECS.h"
#include "../Components/BoxCollidorComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL.h>
class CollidorRectangleRenderSystem: public System {
public:
	CollidorRectangleRenderSystem() {
		RequireComponent<BoxCollidorComponent>();
		RequireComponent<TransformComponent>();
	}
	void Update(SDL_Renderer* renderer, SDL_Rect& camera) {
		for (auto entity : GetSystemEntities()) {
			auto& collidor = entity.GetComponent<BoxCollidorComponent>();
			auto& transform = entity.GetComponent<TransformComponent>();
			SDL_Rect stroke = {
				transform.position.x + collidor.offset.x - camera.x,
				transform.position.y + collidor.offset.y - camera.y, 
				collidor.width * transform.scale.x, 
				collidor.height * transform.scale.y};
			SDL_RenderDrawRect(renderer, &stroke);
		}
	}
};