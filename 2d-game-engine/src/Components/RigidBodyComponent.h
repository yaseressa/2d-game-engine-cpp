#pragma once
#include <../libs/glm/glm.hpp>
using namespace glm;
struct RigidBodyComponent {
	vec2 velocity;
	RigidBodyComponent(vec2 velocity = vec2(0, 0)) :velocity(velocity) {};
};