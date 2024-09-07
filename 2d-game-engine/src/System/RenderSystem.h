#pragma once
#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL.h>
#include <iostream>
#include <algorithm>
using namespace std;
class RenderSystem : public System {
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& AssetManager) {
		auto sortedEntities = GetSystemEntities();
		sort(sortedEntities.begin(), sortedEntities.end(), [](Entity A, Entity B){
			return A.GetComponent<SpriteComponent>().zIndex < B.GetComponent<SpriteComponent>().zIndex;
            });
        for (auto entity : sortedEntities) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();

            SDL_Rect srcRect = sprite.srcRect;

            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x),
                static_cast<int>(transform.position.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)
            };

            SDL_RenderCopyEx(
                renderer,
                AssetManager->GetTexture(sprite.assetID),
                &srcRect,
                &dstRect,
                transform.rotation,
                NULL,
                SDL_FLIP_NONE
            );
        }
    }
};
