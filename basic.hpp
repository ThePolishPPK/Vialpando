#ifndef SHAPES_H
#define SHAPES_H

#include <imgui.h>

#include <vector>

float angleBetweenPoints(const ImVec2& from, const ImVec2& to);
void drawArrow(const ImVec2& start, const ImVec2& end, ImDrawList* drawList,
			   float arrowLength = 10.0f, float arrowAngle = 30.0f,
			   float thickness = 3.0f, ImColor color = ImColor(255, 255, 255));

struct Force {
	double power = 0;	 // In Newtons
	float angle = 0.0f;	 // In Radians, starting like unit circle
	Force(double power = 0, float angle = 0) : power(power), angle(angle){};
};
Force resultantOfForces(const std::vector<Force>& forces);

struct object {
	ImVec2 position = ImVec2(0.0f, 0.0f);
	ImVec2 move = ImVec2(0.0f, 0.0f);
	float mass = 1.0f;
	float rotate = 0.0f;
	std::vector<Force> forces;
	bool operator==(const object& obj);
};

#endif