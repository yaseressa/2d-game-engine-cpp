#pragma once
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include <string>
class MovementSystem : public System
{
public:
	MovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}
	void Update(double deltaTime) {
		for (auto entity : GetSystemEntities()) {
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidBodyComponent = entity.GetComponent<RigidBodyComponent>();
			transform.position += rigidBodyComponent.velocity * vec2(deltaTime, deltaTime);
			//Logger::Info("position" + to_string(transform.position.x));
		}
	}
};
