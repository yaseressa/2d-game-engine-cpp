#pragma once
#include "../ECS/ECS.h"
#include "../Components/BoxCollidorComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
#include <string>
class DamageSystem : public System {
public:
	DamageSystem() {
		RequireComponent<BoxCollidorComponent>();
		RequireComponent<HealthComponent>();
	}
	void SubscribeToEvents(unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent, DamageSystem>(this, &DamageSystem::onCollision);

	}
	void onCollision(CollisionEvent& event) {
		auto entityA = event.entityA;
		auto entityB = event.entityB;
		if (entityA.HasGroup("projectiles") && entityB.HasTag("player")) {
			onProjectileHitPlayer(entityA, entityB);
		}
		if (entityB.HasGroup("projectiles") && entityA.HasTag("player")) {
			onProjectileHitPlayer(entityB, entityA);

		}
		if (entityA.HasGroup("projectiles") && entityB.HasGroup("enemies")) {
			onProjectileHitEnemy(entityA, entityB);
		}
		if (entityB.HasGroup("projectiles") && entityA.HasGroup("enemies")) {
			onProjectileHitEnemy(entityB, entityA);

		}

	}
	void onProjectileHitPlayer(Entity projectile, Entity player) {
		const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
		if (!projectileComponent.isFriendly) {
			auto& playerHealth = player.GetComponent<HealthComponent>();
			playerHealth.healthPercentage -= projectileComponent.hitPercentageDamage;
			if (playerHealth.healthPercentage <= 0) {
				player.Kill();
			}
			projectile.Kill();
		}

	}
	void onProjectileHitEnemy(Entity projectile, Entity enemy) {
		const auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
		if (projectileComponent.isFriendly) {
			auto& enemyHealth = enemy.GetComponent<HealthComponent>();
			enemyHealth.healthPercentage -= projectileComponent.hitPercentageDamage;
			if (enemyHealth.healthPercentage <= 0) {
				enemy.Kill();
			}
			projectile.Kill();
		}

	}
	void Update() {

	}
};
