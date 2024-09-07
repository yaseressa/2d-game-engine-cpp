#pragma once
#include <SDL.h>
struct AnimationComponent {
	int numFrames;
	int currentFrame;
	int frameRateSpeed;
	bool isLoop;
	int startTime;

	AnimationComponent(int numFrames = 1, int frameRateSpeed = 1, bool isLoop = true)
		: numFrames(numFrames), frameRateSpeed(frameRateSpeed), isLoop(isLoop) {
		this->currentFrame = 1;
		this->startTime = SDL_GetTicks();
	}
};