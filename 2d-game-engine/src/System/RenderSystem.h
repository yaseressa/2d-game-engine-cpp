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

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& AssetManager, SDL_Rect& camera) {
		auto sortedEntities = GetSystemEntities();
		sort(sortedEntities.begin(), sortedEntities.end(), [](Entity A, Entity B){
			return A.GetComponent<SpriteComponent>().zIndex < B.GetComponent<SpriteComponent>().zIndex;
            });

        vector<Entity> insideEntities;

        for (auto entity : sortedEntities) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();

            bool isEntityOutsideCameraView = (
                transform.position.x + (sprite.width * transform.scale.x) < camera.x ||
                transform.position.x > camera.x + camera.w ||
                transform.position.y + (sprite.height * transform.scale.y) < camera.y ||
                transform.position.y > camera.y + camera.h
                );
            if (!isEntityOutsideCameraView and sprite.isFixed) {
                insideEntities.emplace_back(entity);
            };
        }
        for (auto entity : sortedEntities) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();

            SDL_Rect srcRect = sprite.srcRect;

            SDL_Rect dstRect = {
                static_cast<int>(sprite.isFixed ? transform.position.x : transform.position.x - camera.x),
                static_cast<int>(sprite.isFixed ? transform.position.y : transform.position.y - camera.y),
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
                sprite.flip
            );
        }
    }
};
