#pragma once
#include "../ECS/ECS.h"
#include <glm/glm.hpp>
using namespace glm;
struct KeyboardMovementComponent{
	vec2 upVelocity;
	vec2 leftVelocity;
	vec2 rightVelocity;
	vec2 downVelocity;
	KeyboardMovementComponent(vec2 upVelocity = vec2(0), vec2 leftVelocity = vec2(0), vec2 rightVelocity = vec2(0), vec2 downVelocity = vec2(0)): upVelocity(upVelocity), leftVelocity(leftVelocity), rightVelocity(rightVelocity), downVelocity(downVelocity) {

	}
};
