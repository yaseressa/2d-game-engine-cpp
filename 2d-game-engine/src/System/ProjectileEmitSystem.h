#pragma once
#include "../ECS/ECS.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <string.h>
class ProjectileEmitSystem : public System {
public:
	ProjectileEmitSystem() {
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
		//RequireComponent<RigidBodyComponent>();
	}
	void SubscribeToEvents(unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent, ProjectileEmitSystem>(this, &ProjectileEmitSystem::onSpace);
	}
	void onSpace(KeyPressedEvent& keyPressedEvent) {
		if (keyPressedEvent.key == SDLK_SPACE) {
			for (auto entity : GetSystemEntities()) {
				auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const auto transform = entity.GetComponent<TransformComponent>();
				const auto rigidBody = entity.GetComponent<RigidBodyComponent>();
				if (entity.HasTag("player")) {
					vec2 projectilePosition = transform.position;
					if (entity.HasComponent<SpriteComponent>()) {
						const auto sprite = entity.GetComponent<SpriteComponent>();
						projectilePosition.x += (transform.scale.x * sprite.width) / 2;
						projectilePosition.y += (transform.scale.y * sprite.height) / 2;
					}
					vec2 pv = projectileEmitter.projectileVelocity;
					pv.x *= (rigidBody.velocity.x > 0 ? 1 : rigidBody.velocity.x < 0 ? -1 : 0);
					pv.y *= (rigidBody.velocity.y > 0 ? 1 : rigidBody.velocity.y < 0 ? -1 : 0);

					Entity projectile = entity.worldCordinator->CreateEntity();
					projectile.Group("projectiles");
					projectile.AddComponent<TransformComponent>(projectilePosition, vec2(1, 1), 0);
					projectile.AddComponent<RigidBodyComponent>(pv);
					projectile.AddComponent<SpriteComponent>("bullet-texture", 4, 4, 4);
					projectile.AddComponent<BoxCollidorComponent>(4, 4);
					projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentageDamage, projectileEmitter.projectileDuration);
					projectileEmitter.lastEmissionTime = SDL_GetTicks();
				}
			}
		}
	}
	void Update(unique_ptr<WorldCordinator>& worldCordinator) {
		
		for (auto entity : GetSystemEntities()) {
			auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			const auto transform = entity.GetComponent<TransformComponent>();
			if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency and entity.HasGroup("enemies")) {
				vec2 projectilePosition = transform.position;
				if (entity.HasComponent<SpriteComponent>()){
					const auto sprite = entity.GetComponent<SpriteComponent>();
					projectilePosition.x += (transform.scale.x * sprite.width) / 2;
					projectilePosition.y += (transform.scale.y * sprite.height) / 2;

				}
				Entity projectile = worldCordinator->CreateEntity();
				projectile.Group("projectiles");
				projectile.AddComponent<TransformComponent>(projectilePosition, vec2(1, 1), 0);
				projectile.AddComponent<RigidBodyComponent>(projectileEmitter.projectileVelocity);
				projectile.AddComponent<SpriteComponent>("bullet-texture", 4, 4, 4);
				projectile.AddComponent<BoxCollidorComponent>(4, 4);
				projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentageDamage, projectileEmitter.projectileDuration);
				projectileEmitter.lastEmissionTime = SDL_GetTicks();
			}
		} 
	}
};