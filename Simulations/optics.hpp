#ifndef OPTICS_H
#define OPTICS_H

#include <imgui.h>

#include <cmath>

#include "../basic.hpp"
#include "../view.hpp"

struct Lens {
	ImVec2 position = {0, 0};  // Position in meters form top-left screen.
	ImVec2 begin, end;
	float height = 0.3f;	 // Lens height
	float angle = M_PI / 2;	 // Angle starts like in unit circle
	float principalFocus = 0.256f;
	enum Type {	 // https://en.wikipedia.org/wiki/Lens#/media/File:Lenses_en.svg
		biconvex,
		biconcave
	};
	Type type;
	bool operator==(const Lens l) {
		return (this->position.x == l.position.x &&
				this->position.y == l.position.y && this->height == l.height &&
				this->angle == l.angle &&
				this->principalFocus == l.principalFocus &&
				this->type == l.type);
	}
};

class Optics : public View {
   public:
	Optics();
	void draw();

   private:
};

#endif