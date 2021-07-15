#include "gravity.hpp"

#include <imgui.h>
#include <stdio.h>

#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#define GRAVITY_G 6.67430e-11
#define EARTH_MASS 5.97219e24
#define EARTH_RADIUS 6371008

// FIXME: If objects goes on center other, then is infinite accelerated out

float Gravity::sensitivity = 16.0f;

Gravity::Gravity() {
	this->keepActive = false;
	this->name = "Gravity";
	this->scale = 65536.0;
	this->forceScale = 0.11;
	this->viewX = 300;
	this->viewY = 300;
	this->drawForceVectors = true;
	this->drawAxes = true;
	this->vectorThickness = 3.0;
	this->arrowAngle = 50.0;
	this->arrowLength = 8.0;
	this->forceColor = ImColor(0, 255, 251);
	this->axesColor = ImColor(255, 255, 0);
	this->axesStepsColor = ImColor(64, 255, 16);
	this->lastMoveTime = ImGui::GetTime();
	this->timeSpeed = 1.0;
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
			ImGui::DragFloat("Szybkość czasu", &this->timeSpeed,
							 std::pow(2, 14), 0, std::pow(2, 24), "%.2f",
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
			ImGui::ColorEdit3("Kolor osi", (float*)&this->axesColor);
			ImGui::ColorEdit3("Kolor skali na osi",
							  (float*)&this->axesStepsColor);
			ImGui::Checkbox("Wektory sił", &this->drawForceVectors);
			ImGui::Checkbox("Rysuj osie", &this->drawAxes);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Obiekty", true)) {
			unsigned char number = 1;
			for (auto& object : this->objects) {
				std::string name(std::string("Obiekt ") +
								 std::to_string(number));
				if (ImGui::BeginMenu(name.c_str())) {
					Gravity::editObjectMenu(object.mass, object.radius,
											object.move.speedX,
											object.move.speedY);
					ImGui::Text("Pozycja:\n\tX: %e m\n\tY: %e m",
								object.position.x, object.position.y);
					ImGui::EndMenu();
				}
				number++;
			}
			if (ImGui::Button("Dodaj nowy obiekt")) {
				ImGui::OpenPopup("AddNewObject");
			}
			if (ImGui::BeginPopupModal("AddNewObject", NULL,
									   ImGuiWindowFlags_NoMove)) {
				static float radius = 0.0f, speedX = 0.0f, speedY = 0.0f;
				static double mass = 0.0f, x = 0, y = 9;
				static double min = std::numeric_limits<double>::lowest();
				static double max = std::numeric_limits<double>::max();

				Gravity::editObjectMenu(mass, radius, speedX, speedY);
				ImGui::DragScalar("Pozycja X", ImGuiDataType_Double, &x,
								  std::abs(x) / Gravity::sensitivity + 0.01f,
								  &min, &max, "%e m");
				ImGui::DragScalar("Pozycja Y", ImGuiDataType_Double, &y,
								  std::abs(y) / Gravity::sensitivity + 0.01f,
								  &min, &max, "%e m");

				if (ImGui::Button("Dodaj")) {
					Gravity::object obj;
					obj.radius = radius;
					obj.mass = mass;
					obj.move.speedX = speedX;
					obj.move.speedY = speedY;
					obj.position = Gravity::point(x, y);
					this->objects.push_back(obj);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset")) {
					radius = 0;
					speedX = 0;
					speedY = 0;
					mass = 0;
				}
				ImGui::SameLine();
				if (ImGui::Button("Anuluj")) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
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

		// TODO: Repair drawing vectors under objects
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

	if (ImGui::GetTime() - this->lastMoveTime < 1.0) {
		double part =
			(ImGui::GetTime() - this->lastMoveTime) / (1 / this->timeSpeed);
		for (auto& obj : this->objects) {
			for (auto& force : obj.forcesVector) {
				double speed = force.power / obj.mass;
				obj.move.speedX += speed * std::cos(force.angle) * part;
				obj.move.speedY += speed * std::sin(force.angle) * part;
			}
			obj.position.x += obj.move.speedX * part;
			obj.position.y += obj.move.speedY * part;
		}
	}
	this->lastMoveTime = ImGui::GetTime();

	for (auto& obj : this->objects) {
		obj.forcesVector.clear();
		for (auto& grav : this->objects) {
			if (grav.position.x != obj.position.x ||
				grav.position.y != obj.position.y || grav.mass != obj.mass) {
				obj.forcesVector.push_back(this->calcGravityForce(obj, grav));
			}
		}
	}

	if (this->drawAxes) {
		double step;
		int length;
		int drawX, drawY;
		int exp;
		exp = std::floor(std::log10(this->scale));
		step = std::ceil(this->scale / std::pow(10, exp) / 5) *
			   std::pow(10, exp) * 500;
		length = step * (1 / this->scale);

		drawY = this->viewX >= 0 && this->viewX < windowSize.x
					? this->viewX
					: (this->viewX < 0 ? 0 : windowSize.x - 1);
		drawX = this->viewY >= 0 && this->viewY < windowSize.y
					? this->viewY
					: (this->viewY < 0 ? 0 : windowSize.y - 1);

		list->AddLine(ImVec2(p0.x + drawY, p0.y),
					  ImVec2(p0.x + drawY, p0.y + windowSize.y),
					  this->axesColor, 3);
		list->AddLine(ImVec2(p0.x, p0.y + drawX),
					  ImVec2(p0.x + windowSize.x, p0.y + drawX),
					  this->axesColor, 3);

		// TODO: Convert two loops into one
		for (int x = -(int)std::floor(this->viewX / (double)length) - 1,
				 pos = x * length;
			 pos <= -this->viewX + windowSize.x + (2 * length); x++) {
			char label[12];
			std::snprintf(&label[0], 12, "%.1em", x * step);
			list->AddText(ImVec2(p0.x + pos + this->viewX, p0.y + drawX + 20),
						  this->axesStepsColor, &label[0]);
			pos += length;
		}

		for (int y = -(int)std::floor(this->viewY / (double)length) - 1,
				 pos = y * length;
			 pos <= -this->viewY + windowSize.y + (2 * length); y++) {
			char label[12];
			std::snprintf(&label[0], 12, "%.1em", y * step);
			list->AddText(ImVec2(p0.x + drawY, p0.y + pos + this->viewY),
						  this->axesStepsColor, &label[0]);
			pos += length;
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
	object2.position = point(0, EARTH_RADIUS * 2);
	object2.radius = EARTH_RADIUS * 0.2;
	object2.move.speedX =  // Orbital Speed for object1
		std::sqrt(GRAVITY_G * object1.mass / (EARTH_RADIUS * 2));

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
	float angle = this->angleBetweenPoints(end, start);
	float angleDiff = this->arrowAngle / 180 * M_PI / 2;
	drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y),
					  this->forceColor, this->vectorThickness);
	ImVec2 arrow[] = {
		ImVec2(end.x + this->arrowLength * std::cos(angle + angleDiff),
			   end.y + this->arrowLength * std::sin(angle + angleDiff)),
		ImVec2(end.x, end.y),
		ImVec2(end.x + this->arrowLength * std::cos(angle - angleDiff),
			   end.y + this->arrowLength * std::sin(angle - angleDiff))};
	drawList->AddPolyline((ImVec2*)&arrow, 3, this->forceColor, false,
						  this->vectorThickness);
}

void Gravity::editObjectMenu(double& mass, float& radius, float& speedX,
							 float& speedY) {
	// TODO: Incress accuranct
	double min = 0.001f, max = std::numeric_limits<double>::max();
	ImGui::DragScalar("Masa", ImGuiDataType_Double, &mass,
					  std::abs(mass) / Gravity::sensitivity + 0.01f, &min, &max,
					  "%e kg");
	ImGui::DragFloat("Promień", &radius,
					 std::abs(radius) / Gravity::sensitivity + 0.01f, 0.0001f,
					 std::numeric_limits<float>::max(), "% .4f m",
					 ImGuiSliderFlags_NoRoundToFormat);
	ImGui::DragFloat(
		"Prędkość X", &speedX, std::abs(speedX) / Gravity::sensitivity + 0.01f,
		std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(),
		"% .3f m/s", ImGuiSliderFlags_NoRoundToFormat);
	ImGui::DragFloat(
		"Prędkość Y", &speedY, std::abs(speedY) / Gravity::sensitivity + 0.01f,
		std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(),
		"% .3f m/s", ImGuiSliderFlags_NoRoundToFormat);
	ImGui::Text("Prędkość całkowita: % .2f m/s",
				std::sqrt(std::pow(speedX, 2) + std::pow(speedY, 2)));
}