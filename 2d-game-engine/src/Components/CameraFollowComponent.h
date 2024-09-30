#pragma once

struct CameraFollowComponent {
	int h, w, x, y;
	CameraFollowComponent(int x = 0, int y = 0, int h = 0, int w = 0) : x(x), y(y), h(h), w(w) {} 
};
