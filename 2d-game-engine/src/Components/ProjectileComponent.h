#pragma once
#include <SDL.h>
struct ProjectileComponent {
	bool isFriendly;
	int hitPercentageDamage;
	int duration;
	int startTime;
	ProjectileComponent(bool isFriendly = false, int hitPercentageDamage = 0, int duration = 0): isFriendly(isFriendly), hitPercentageDamage(hitPercentageDamage), duration(duration) {
		startTime = SDL_GetTicks();
	}
};