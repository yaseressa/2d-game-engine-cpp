#pragma once
#include "../ECS/ECS.h"
#include "../Components/BoxCollidorComponent.h"
#include "../Components/TransformComponent.h"
#include <glm/glm.hpp>
#include <SDL.h>
using namespace glm;
using namespace std;
class CollisionSystem : public System {
public:
	CollisionSystem() {
		RequireComponent<BoxCollidorComponent>();
		RequireComponent<TransformComponent>();

	}
	void Update(SDL_Renderer* renderer){
		SDL_SetRenderDrawColor(renderer, 255, 255, 2, 1);
		auto entities = GetSystemEntities();
		for (auto entityA = entities.begin(); entityA != entities.end(); entityA++) {
			auto& transformA = entityA->GetComponent<TransformComponent>();
			auto& collideA = entityA->GetComponent<BoxCollidorComponent>();
			for (auto entityB = entityA + 1; entityB < entities.end(); entityB++) {
				auto& transformB = entityB->GetComponent<TransformComponent>();
				auto& collideB = entityB->GetComponent<BoxCollidorComponent>();
				if (transformA.position.x + collideA.offset.x < transformB.position.x + collideB.offset.x + collideB.width &&
					transformA.position.x + collideA.offset.x + collideA.width > transformB.position.x + collideB.offset.x &&
					transformA.position.y + collideA.offset.y < transformB.position.y + collideB.offset.y + collideB.height &&
					transformA.position.y + collideA.offset.y + collideA.height > transformB.position.y + collideB.offset.y)
				{
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
					entityA->Kill();
					entityB->Kill();
				}
			}
		}
	}
};