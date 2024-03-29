#ifndef GRAVITY_H
#define GRAVITY_H

#include <imgui.h>

#include <limits>
#include <vector>

#include "../basic.hpp"
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
	class object {
	   public:
		ImVec2 position;
		float radius = 1;
		float mass = 0.1;
		ImColor color = ImColor(255, 0, 0);
		struct {
			float speedX = 0;  // In m/s
			float speedY = 0;  // In m/s
		} move;
		std::vector<Force> forcesVector;
		void simplify();
		bool operator==(const object& obj);
	};
	std::vector<object> objects;
	Force calcGravityForce(const object& o1, const object& o2);
	static void editObjectMenu(float& mass, float& radius, float& speedX,
							   float& speedY);
};

#endif