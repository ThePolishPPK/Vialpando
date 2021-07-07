#ifndef GRAVITY_H
#define GRAVITY_H

#include <imgui.h>

#include <vector>

#include "../view.hpp"

class Gravity : public View {
   public:
	Gravity();
	void draw();

   private:
	ImVec2 lastDrawingLocation;
	float scale;
	float forceScale;
	float vectorThickness;
	float arrowAngle;
	float arrowLength;
	ImColor forceColor;
	bool drawForceVectors;
	int viewX, viewY;
	void reset();
	struct Force {
		double power;  // In Newtons
		float angle;   // In Radians, starting like unit circle
	};
	struct point {
		double x;
		double y;
		point(double x = 0, double y = 0) : x(x), y(y){};
	};
	class object {
	   public:
		point position;
		double radius;
		double mass;
		std::vector<Force> forcesVector;
		Force resultantOfForces();
		void simplify();
	};
	std::vector<object> objects;
	Force calcGravityForce(const object& o1, const object& o2);
	static float angleBetweenPoints(const point& from, const point& to);
	void drawArrow(const point& start, const point& end, ImDrawList* drawList);
};

#endif