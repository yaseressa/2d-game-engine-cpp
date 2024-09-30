#pragma once
#include "../EventBus/Event.h"
#include "SDL.h"
class KeyPressedEvent : public Event {
public:
	SDL_Keycode key;
	KeyPressedEvent(SDL_Keycode key): key(key) {

	}
};