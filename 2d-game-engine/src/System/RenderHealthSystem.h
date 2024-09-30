#pragma once
#include "../ECS/ECS.h"
#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL_ttf.h>
#include "../AssetManager/AssetManager.h"
#include <string>
class RenderHealthSystem : public System {
public:
	RenderHealthSystem() {
		RequireComponent<HealthComponent>();
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}
	void Update(SDL_Renderer* renderer, unique_ptr<AssetManager>& assetManager, const SDL_Rect camera) {
		for (auto entity : GetSystemEntities()) {
			const auto healthComponent = entity.GetComponent<HealthComponent>();
			const auto transformComponent = entity.GetComponent<TransformComponent>();
			const auto spriteComponent = entity.GetComponent<SpriteComponent>();

			SDL_Color color;
			if (healthComponent.healthPercentage  >= 75) {
				color = { 0, 255, 0 };
			}else if (healthComponent.healthPercentage  >= 30) {
				color = { 255, 255, 0 };
			}else {
				color = { 255, 0, 0 };
			}
			SDL_Surface* surface = TTF_RenderText_Blended(assetManager->GetFont("pico8-font-5"), to_string(healthComponent.healthPercentage).c_str(), color);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			int hBWidth = 15, hBHeight = 3, labelWidth = 0, labelHeight = 0;
			SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
			SDL_Rect dstRectTxt = {
				static_cast<int>((transformComponent.position.x + (spriteComponent.width * transformComponent.scale.x)) - camera.x),
				static_cast<int>(transformComponent.position.y - camera.y + 5),
				labelWidth,
				labelHeight
			};
			SDL_Rect dstRectBar = {
				static_cast<int>((transformComponent.position.x + (spriteComponent.width * transformComponent.scale.x)) - camera.x),
				static_cast<int>(transformComponent.position.y - camera.y),
				static_cast<int>(hBWidth * (healthComponent.healthPercentage / 100.0)),
				hBHeight
			};
			SDL_RenderCopy(renderer, texture, NULL, &dstRectTxt);
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
			SDL_RenderFillRect(renderer, &dstRectBar);
			SDL_DestroyTexture(texture);
		}
	}
};