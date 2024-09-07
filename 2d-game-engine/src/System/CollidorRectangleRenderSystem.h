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
	void Update(SDL_Renderer* renderer) {
		for (auto entity : GetSystemEntities()) {
			auto& collidor = entity.GetComponent<BoxCollidorComponent>();
			auto& transform = entity.GetComponent<TransformComponent>();
			SDL_Rect stroke = {transform.position.x + collidor.offset.x, transform.position.y + collidor.offset.y, collidor.width, collidor.height};
			SDL_RenderDrawRect(renderer, &stroke);
		}
	}
};