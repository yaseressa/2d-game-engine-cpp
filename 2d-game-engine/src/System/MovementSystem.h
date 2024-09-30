#pragma once
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Events/CollisionEvent.h"
#include <string.h>
class MovementSystem : public System
{
public:
	MovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}
	void SubscribeToEvents(unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent, MovementSystem>(this, &MovementSystem::onCollision);

	}
	void onCollision(CollisionEvent& event) {
		Entity& entityA = event.entityA;
		Entity& entityB = event.entityB;
		Logger::Warn(to_string(entityA.HasGroup("enemies")) + "   :   " + to_string(entityB.HasGroup("obstacles")));
		if (entityA.HasGroup("obstacles") and entityB.HasGroup("enemies")) {
			Logger::Fatal("collided");
			onObstacle(entityB, entityA);
		}
		if (entityA.HasGroup("enemies") and entityB.HasGroup("obstacles")) {
			Logger::Fatal("collided");
			onObstacle(entityA, entityB);
			
		}

	}
	void onObstacle(Entity enemy, Entity obstacle) {
		if (enemy.HasComponent<RigidBodyComponent>() and enemy.HasComponent<SpriteComponent>()) {
			auto& rigid = enemy.GetComponent<RigidBodyComponent>();
			auto& enemyTransform = enemy.GetComponent<TransformComponent>();
			auto& obstacleTransform = obstacle.GetComponent<TransformComponent>();
			auto& sprite = enemy.GetComponent<SpriteComponent>();
			if (rigid.velocity.x != 0) {
				rigid.velocity.x = -1 * (rigid.velocity.x);
				sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}
			if (rigid.velocity.y != 0) {
				rigid.velocity.y = -1 * (rigid.velocity.y);
				sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}
	}
	void Update(double deltaTime) {
		for (auto entity : GetSystemEntities()) {
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();
			transform.position += rigidBodyComponent.velocity * vec2(deltaTime, deltaTime);
			//Logger::Info("position" + to_string(transform.position.x));
			bool OutsideTheMap = (transform.position.x < 0 || transform.position.x > Game::MapWidth) ||
				(transform.position.y < 0 || transform.position.y > Game::MapHeight);
			if (OutsideTheMap && !entity.HasTag("player")) {
				entity.Kill();
			}
			if (entity.HasTag("player")) {
				int pl = 10, pt = 10, pr = 50, pb = 50;
				transform.position.x = transform.position.x < pl ? pl : transform.position.x;
				transform.position.x = transform.position.x > Game::MapWidth - pr ? Game::MapWidth - pr : transform.position.x;
				transform.position.y = transform.position.y < pt ? pt : transform.position.y;
				transform.position.y = transform.position.y > Game::MapHeight - pb ? Game::MapHeight - pb : transform.position.y;
			}
		}
	}
};
