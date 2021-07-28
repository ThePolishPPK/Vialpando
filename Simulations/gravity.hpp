#ifndef GRAVITY_H
#define GRAVITY_H

#include <imgui.h>

#include <limits>
#include <vector>

#include "../view.hpp"

class Gravity : public View {
   public:
	Gravity();
	void draw();
	static float sensitivity;

   private:
	ImVec2 lastDrawingLocation;
	float scale;
	float forceScale;
	float vectorThickness;
	float arrowAngle;
	float arrowLength;
	ImColor forceColor;
	ImColor axesColor;
	ImColor axesStepsColor;
	bool drawForceVectors;
	bool drawAxes;
	int viewX, viewY;
	double lastMoveTime;
	float timeSpeed;
	void reset();
	struct Force {
		double power;  // In Newtons
		float angle;   // In Radians, starting like unit circle
		Force(double power = 0, float angle = 0) : power(power), angle(angle){};
	};
	class object {
	   public:
		ImVec2 position;
		float radius;
		double mass;
		ImColor color = ImColor(255, 0, 0);
		struct {
			float speedX = 0;  // In m/s
			float speedY = 0;  // In m/s
		} move;
		std::vector<Force> forcesVector;
		Force resultantOfForces();
		void simplify();
	};
	std::vector<object> objects;
	Force calcGravityForce(const object& o1, const object& o2);
	static void editObjectMenu(double& mass, float& radius, float& speedX,
							   float& speedY);
};

#endif