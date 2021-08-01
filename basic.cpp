#include "basic.hpp"

#include <imgui.h>

#include <cmath>
#include <vector>

float angleBetweenPoints(const ImVec2& from, const ImVec2& to) {
	double x = to.x - from.x;
	double y = to.y - from.y;
	double c = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
	float result = 0;

	if (x * y == 0) {
		result = (x != 0) ? std::acos(x / c) : std::asin(y / c);
	} else {
		result = std::asin(std::abs(y) / c);
		if (y > 0) {
			if (x < 0) result = M_PI - result;
		} else {
			if (x < 0) {
				result += M_PI;
			} else {
				result = 2 * M_PI - result;
			}
		}
	}
	return result;
}

void drawArrow(const ImVec2& start, const ImVec2& end, ImDrawList* drawList,
			   float arrowLength, float arrowAngle, float thickness,
			   ImColor color) {
	float angle = angleBetweenPoints(end, start);
	float angleDiff = (arrowAngle / 180) * M_PI / 2;
	drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), color,
					  thickness);
	ImVec2 arrow[] = {
		ImVec2(end.x + arrowLength * std::cos(angle + angleDiff),
			   end.y + arrowLength * std::sin(angle + angleDiff)),
		ImVec2(end.x, end.y),
		ImVec2(end.x + arrowLength * std::cos(angle - angleDiff),
			   end.y + arrowLength * std::sin(angle - angleDiff))};
	drawList->AddPolyline((ImVec2*)&arrow, 3, color, false, thickness);
}

Force resultantOfForces(const std::vector<Force>& forces) {
	double x = 0, y = 0;

	for (auto& force : forces) {
		y += std::sin(force.angle) * force.power;
		x += std::cos(force.angle) * force.power;
	}

	Force result;
	if (x != 0 && y != 0) {
		result.power = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
		result.angle = angleBetweenPoints(ImVec2(0, 0), ImVec2(x, y));
	}
	return result;
}

bool object::operator==(const object& obj) {
	if (this->mass == obj.mass && this->position.x == obj.position.x &&
		this->position.y == obj.position.y && this->move.x == obj.move.x &&
		this->move.y == obj.move.y && this->rotate == obj.rotate)
		return true;
	return false;
}