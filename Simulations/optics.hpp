#ifndef OPTICS_H
#define OPTICS_H

#include <imgui.h>

#include <cmath>
#include <vector>

#include "../basic.hpp"
#include "../view.hpp"

namespace Optic {
struct Ray {
	std::vector<ImVec2> points;
	float _angle = 0;  // Circle unit angle in radians for calculations only.
};

struct Lens {
	ImVec2 position = {0, 0};
	ImVec2 begin, end;
	float height = 0.3f;
	float angle = M_PI / 2;
	float angleSize = M_PI / 2;
	float principalFocus = 0.256f;
	enum Type {	 // https://en.wikipedia.org/wiki/Lens#/media/File:Lenses_en.svg
		biconvex,
		biconcave,
		concaveReflector
	};
	Type type;
	bool operator==(const Lens l) {
		return (this->position.x == l.position.x &&
				this->position.y == l.position.y && this->height == l.height &&
				this->angle == l.angle &&
				this->principalFocus == l.principalFocus &&
				this->type == l.type);
	}
	ImVec2 getHitPoint(const Ray& r);
};
}  // namespace Optic

class Optics : public View {
   public:
	Optics();
	void draw();

   private:
};

#endif