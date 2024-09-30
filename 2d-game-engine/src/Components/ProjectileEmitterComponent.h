#pragma once
#include <glm/glm.hpp>
using namespace glm;
struct ProjectileEmitterComponent {
	vec2 projectileVelocity;
	int repeatFrequency;
	int projectileDuration;
	int hitPercentageDamage;
	bool isFriendly;
	int lastEmissionTime;

	ProjectileEmitterComponent(
	vec2 projectileVelocity = vec2(0),
	int repeatFrequency = 0,
	int projectileDuration = 1000,
	int hitPercentageDamage = 10,
	bool isFriendly = false): projectileVelocity(projectileVelocity), repeatFrequency(repeatFrequency), projectileDuration(projectileDuration), hitPercentageDamage(hitPercentageDamage), isFriendly(isFriendly){
		this->lastEmissionTime = SDL_GetTicks();
	}
};