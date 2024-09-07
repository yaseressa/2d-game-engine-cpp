#pragma once
#include <glm/glm.hpp>
using namespace glm;
struct BoxCollidorComponent {
	int width, height;
	vec2 offset;
	BoxCollidorComponent(int width = 0, int height = 0, vec2 offset = vec2(0)): width(width), height(height), offset(offset){}
};