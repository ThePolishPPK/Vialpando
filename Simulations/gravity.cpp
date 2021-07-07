#include "gravity.hpp"

#include <imgui.h>
#include <stdio.h>

#include <cmath>
#include <iostream>
#include <limits>
#define GRAVITY_G 6.7e-11
#define EARTH_MASS 5.97219e24
#define EARTH_RADIUS 6371008

Gravity::Gravity() {
	this->keepActive = false;
	this->name = "Gravity";
	this->scale = 65536.0;
	this->forceScale = 0.03;
	this->viewX = 300;
	this->viewY = 300;
	this->drawForceVectors = true;
	this->vectorThickness = 3.0;
	this->arrowAngle = 50.0;
	this->arrowLength = 8.0;
	// this->forceColor = 0x00FF00;
	this->reset();
}

void Gravity::draw() {
	ImDrawList* list;

	ImGui::Begin(this->name, &this->keepActive, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Opcje", true)) {
			ImGui::DragFloat("Skala wielkości", &this->scale, 16, 0,
							 std::pow(2, 18), "%.7f",
							 ImGuiSliderFlags_Logarithmic);
			ImGui::DragFloat("Skala wektorów", &this->forceScale, 1, 0,
							 std::pow(2, 10), "%.7f",
							 ImGuiSliderFlags_Logarithmic);
			ImGui::DragFloat("Grubość lini wektorów", &this->vectorThickness,
							 0.02, 0, 32, "%.0f");
			ImGui::DragFloat("Kąt między strzałkami", &this->arrowAngle, 0.08,
							 0, 180, "%.2f");
			ImGui::DragFloat("Długość strzałek", &this->arrowLength, 0.08, 0,
							 128, "%.2f");
			ImGui::ColorEdit3("Kolor wektorów", (float*)&this->forceColor);
			ImGui::Checkbox("Wektory sił", &this->drawForceVectors);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	list = ImGui::GetWindowDrawList();
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 windowSize = ImGui::GetContentRegionAvail();

	ImGui::InvisibleButton("WindowContent",
						   ImVec2((windowSize.x == 0) ? 1 : windowSize.x,
								  (windowSize.y == 0) ? 1 : windowSize.y),
						   ImGuiButtonFlags_MouseButtonLeft);

	// TODO: Add touch support
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsItemFocused() &&
		ImGui::IsItemActive()) {
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		this->viewX += delta.x;
		this->viewY += delta.y;
	}

	for (auto& obj : this->objects) {
		ImVec2 lastDrawing(
			obj.position.x * (1 / this->scale) + p0.x + this->viewX,
			obj.position.y * (1 / this->scale) + p0.y + this->viewY);

		if (obj.radius * (1 / this->scale) < 1 ||
			this->scale < std::numeric_limits<float>::min())
			break;
		float radius = obj.radius * (1 / this->scale);

		if (radius > 16384) {
			radius = 16384;
		}

		list->AddCircleFilled(lastDrawing, radius, ImColor(255, 0, 0, 255));

		if (this->drawForceVectors) {
			for (auto& force : obj.forcesVector) {
				this->drawArrow(
					point(lastDrawing.x, lastDrawing.y),
					point(
						lastDrawing.x + (force.power * (1 / this->forceScale) *
										 std::cos(force.angle)),
						lastDrawing.y + (force.power * (1 / this->forceScale) *
										 std::sin(force.angle))),
					list);
			}
		}
	}
	for (auto& obj : this->objects) {
		obj.forcesVector.clear();
		for (auto& grav : this->objects) {
			if (grav.position.x != obj.position.x ||
				grav.position.y != obj.position.y || grav.mass != obj.mass) {
				obj.forcesVector.push_back(this->calcGravityForce(obj, grav));
			}
		}
	}

	list->PushClipRect(p0, ImVec2(p0.x + windowSize.x, p0.y + windowSize.y));
	ImGui::End();
}

void Gravity::reset() {
	this->objects.clear();
	object object1, object2;

	object1.mass = EARTH_MASS;
	object1.position = point(0, 0);
	object1.radius = EARTH_RADIUS;

	object2.mass = 1;
	object2.position = point(EARTH_RADIUS * 2, EARTH_RADIUS * 1.3);
	object2.radius = EARTH_RADIUS * 0.2;

	this->objects.push_back(object1);
	this->objects.push_back(object2);
}

Gravity::Force Gravity::object::resultantOfForces() {
	double x, y = 0;

	for (auto& force : this->forcesVector) {
		y += std::sin(force.angle) * force.power;
		x += std::cos(force.angle) * force.power;
	}

	Gravity::Force result;
	result.power = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
	result.angle = angleBetweenPoints(point(0, 0), point(x, y));
	return result;
}

void Gravity::object::simplify() {
	Force force = this->resultantOfForces();
	this->forcesVector.clear();
	this->forcesVector.push_back(force);
}

Gravity::Force Gravity::calcGravityForce(const Gravity::object& o1,
										 const Gravity::object& o2) {
	Force force;
	force.power = GRAVITY_G * o1.mass * o2.mass /
				  (std::pow(o1.position.x - o2.position.x, 2) +
				   std::pow(o1.position.y - o2.position.y, 2));
	point p1(o1.position.x + o1.radius / 2, o1.position.y - o1.radius / 2);
	point p2(o2.position.x + o2.radius / 2, o2.position.y - o2.radius / 2);
	force.angle = angleBetweenPoints(o1.position, o2.position);
	return force;
}

float Gravity::angleBetweenPoints(const Gravity::point& from,
								  const Gravity::point& to) {
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

void Gravity::drawArrow(const Gravity::point& start, const Gravity::point& end,
						ImDrawList* drawList) {
	// TODO: Color manage
	float angle = this->angleBetweenPoints(end, start);
	float angleDiff = this->arrowAngle / 180 * M_PI / 2;
	drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y),
					  ImColor(200, 200, 0), this->vectorThickness);
	ImVec2 arrow[] = {
		ImVec2(end.x + this->arrowLength * std::cos(angle + angleDiff),
			   end.y + this->arrowLength * std::sin(angle + angleDiff)),
		ImVec2(end.x, end.y),
		ImVec2(end.x + this->arrowLength * std::cos(angle - angleDiff),
			   end.y + this->arrowLength * std::sin(angle - angleDiff))};
	drawList->AddPolyline((ImVec2*)&arrow, 3, ImColor(200, 200, 0), false,
						  this->vectorThickness);
}