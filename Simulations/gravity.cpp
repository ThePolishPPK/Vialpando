#include "gravity.hpp"

#include <imgui.h>
#include <stdio.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>

#include "../basic.hpp"
#include "../translate.hpp"
#define GRAVITY_G 6.67430e-11
#define EARTH_MASS 5.97219e24
#define EARTH_RADIUS 6371008
#define LIGHT_SPEED 299792458
#define ENVIROMENT_SIZE 8e8

// FIXME: If objects goes on center other, then is infinite accelerated out
// TODO: Improve zoom. Zoom into (0,0) but not in cursor position or window
// center

float Gravity::sensitivity = 16.0f;

Gravity::Gravity() {
	this->keepActive = false;
	this->name = "Gravity";
	this->scale = 65536.0;
	this->forceScale = 0.11;
	this->viewX = 300;
	this->viewY = 300;
	this->drawForceVectors = true;
	this->drawAxes = false;
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

	// Constants
	const static float maxScale = 1 << 18, minScale = 0.05f;

	// Create menubar
	if (ImGui::BeginMenuBar()) {
		// Menu for full enviroment
		if (ImGui::BeginMenu(tr("Options").c_str(), true)) {
			ImGui::DragFloat(
				tr("Scale").c_str(), &this->scale, 128, minScale, maxScale,
				"%.2f m/px",
				ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat(
				tr("Time speed").c_str(), &this->timeSpeed, 5, 0, 1 << 13,
				"%.2f sim(s)/s",
				ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);

			// Force vectors configuration
			ImGui::Checkbox(tr("Force vectors").c_str(),
							&this->drawForceVectors);
			ImGui::SameLine(ImGui::CalcItemWidth());
			if (ImGui::BeginMenu(tr("Vector options").c_str())) {
				ImGui::DragFloat(tr("Scale").c_str(), &this->forceScale, 1,
								 1e-3, 1 << 10, "%.5f N/px",
								 ImGuiSliderFlags_Logarithmic |
									 ImGuiSliderFlags_AlwaysClamp);
				ImGui::DragFloat(tr("Line width").c_str(),
								 &this->vectorThickness, 0.02, 0, 16, "%.0f px",
								 ImGuiSliderFlags_AlwaysClamp);
				ImGui::DragFloat(tr("Angle between arrow").c_str(),
								 &this->arrowAngle, 0.08, 0, 180, "%.2f°",
								 ImGuiSliderFlags_AlwaysClamp);
				ImGui::DragFloat(tr("Arrow length").c_str(), &this->arrowLength,
								 0.08, 0, 128, "%.0f px",
								 ImGuiSliderFlags_AlwaysClamp);
				ImGui::ColorEdit3(tr("Color").c_str(),
								  (float*)&this->forceColor);
				ImGui::EndMenu();
			}

			// Axes configuration
			ImGui::Checkbox(tr("Axes").c_str(), &this->drawAxes);
			ImGui::SameLine(ImGui::CalcItemWidth());
			if (ImGui::BeginMenu(tr("Axis options").c_str())) {
				ImGui::ColorEdit3(tr("Axis color").c_str(),
								  (float*)&this->axesColor);
				ImGui::ColorEdit3(tr("Labels color").c_str(),
								  (float*)&this->axesStepsColor);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		// Menu to edit each of objects
		if (ImGui::BeginMenu(tr("Objects").c_str(), true)) {
			unsigned char number = 1;
			const static double posMin = std::numeric_limits<double>::lowest(),
								posMax = std::numeric_limits<double>::max();

			// Loop making submenu for each of object
			for (auto& object : this->objects) {
				std::string name(tr("Object") + " " + std::to_string(number));
				if (ImGui::BeginMenu(name.c_str())) {
					Gravity::editObjectMenu(object.mass, object.radius,
											object.move.speedX,
											object.move.speedY);
					ImGui::ColorEdit3(tr("Color").c_str(),
									  (float*)&object.color);
					ImGui::EndMenu();
				}
				number++;
			}

			if (ImGui::Button(tr("Add new object").c_str())) {
				ImGui::OpenPopup(tr("New object").c_str());
			}

			// Menu for new object
			if (ImGui::BeginPopupModal(tr("New object").c_str(), NULL,
									   ImGuiWindowFlags_NoMove)) {
				static float radius = 1.0f, speedX = 0.0f, speedY = 0.0f,
							 mass = 1.0f;
				static double x = 0.0, y = 0.0;
				static ImColor color = ImColor(255, 0, 0);

				Gravity::editObjectMenu(mass, radius, speedX, speedY);
				ImGui::ColorEdit3(tr("Color").c_str(), (float*)&color);

				if (ImGui::Button(tr("Add").c_str())) {
					Gravity::object obj;
					obj.radius = radius;
					obj.mass = mass;
					obj.move.speedX = speedX;
					obj.move.speedY = speedY;
					obj.position = ImVec2(x, y);
					obj.color = color;
					this->objects.push_back(obj);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button(tr("Reset").c_str())) {
					radius = 1.0f;
					speedX = 0.0;
					speedY = 0.0;
					mass = 1.0;
					color = ImColor(255, 0, 0);
				}
				ImGui::SameLine();
				if (ImGui::Button(tr("Cancel").c_str()))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Window info
	list = ImGui::GetWindowDrawList();
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 cursorPos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetContentRegionAvail();

	// Mouse position normalize to frame of reference
	cursorPos.x -= p0.x + this->viewX;
	cursorPos.x *= this->scale;

	cursorPos.y -= p0.y + this->viewY;
	cursorPos.y *= this->scale;

	// Click aera
	ImGui::InvisibleButton("WindowContent",
						   ImVec2((windowSize.x == 0) ? 1 : windowSize.x,
								  (windowSize.y == 0) ? 1 : windowSize.y),
						   ImGuiButtonFlags_MouseButtonLeft);

	// Objects move by cursor
	// TODO: Add touch support
	static object* currentInMove = NULL;
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && currentInMove == NULL) {
		for (auto& obj : objects) {
			if (distanceBetweenPoints(obj.position, cursorPos) <= obj.radius) {
				currentInMove = &obj;
			}
		}
	}
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsItemFocused() &&
		ImGui::IsItemActive()) {
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		if (currentInMove == NULL) {
			this->viewX += delta.x;
			this->viewY += delta.y;
		} else {
			currentInMove->position.x += delta.x * this->scale;
			currentInMove->position.y += delta.y * this->scale;
		}
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) currentInMove = NULL;

	// Edit or add object by mouse
	static object* currentEdited = NULL;
	static bool newObjectIsAdded = false;
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
		currentEdited == NULL) {
		for (auto& obj : objects) {
			if (distanceBetweenPoints(obj.position, cursorPos) <= obj.radius) {
				currentEdited = &obj;
				newObjectIsAdded = false;
			}
		}
		if (currentEdited == NULL) {
			currentEdited = new object();
			currentEdited->position = cursorPos;
			newObjectIsAdded = true;
		}
		ImGui::OpenPopup("ModifyObject");
	}
	if (ImGui::BeginPopupModal(
			"ModifyObject", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		this->editObjectMenu(currentEdited->mass, currentEdited->radius,
							 currentEdited->move.speedX,
							 currentEdited->move.speedY);

		if (ImGui::Button(tr("Remove").c_str())) {
			auto iter = find(this->objects.begin(), this->objects.end(),
							 *currentEdited);
			this->objects.erase(iter);
			currentEdited = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(tr("Save").c_str())) {
			if (newObjectIsAdded) {
				this->objects.push_back(*currentEdited);
			}
			currentEdited = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Screen zoom
	if (ImGui::GetIO().MouseWheel != 0) {
		this->scale *= 1 + (-ImGui::GetIO().MouseWheel / 5);
		if (this->scale > maxScale) {
			this->scale = maxScale;
		} else if (this->scale < minScale)
			this->scale = minScale;
	}

	// Drawing objects on screen
	list->ChannelsSplit(2);

	for (auto& obj : this->objects) {
		ImVec2 lastDrawing(obj.position.x / this->scale + p0.x + this->viewX,
						   obj.position.y / this->scale + p0.y + this->viewY);
		float radius = obj.radius / this->scale;

		list->ChannelsSetCurrent(0);

		list->AddCircleFilled(lastDrawing, radius, obj.color);

		if (this->drawForceVectors) {
			list->ChannelsSetCurrent(1);
			for (auto& force : obj.forcesVector) {
				drawArrow(
					ImVec2(lastDrawing.x, lastDrawing.y),
					ImVec2(
						lastDrawing.x + (force.power * (1 / this->forceScale) *
										 std::cos(force.angle)),
						lastDrawing.y + (force.power * (1 / this->forceScale) *
										 std::sin(force.angle))),
					list, this->arrowLength, this->arrowAngle,
					this->vectorThickness, this->forceColor);
			}
		}
	}
	list->ChannelsMerge();

	// Check objects limitations
	for (auto& obj : this->objects) {
		// Speed check
		float speed = sqrt(pow(obj.move.speedX, 2) + pow(obj.move.speedY, 2));
		if (speed > LIGHT_SPEED) {
			float speedProportion = LIGHT_SPEED / speed;
			obj.move.speedX *= speedProportion;
			obj.move.speedY *= speedProportion;
		}

		// Position
		if (fabs(obj.position.x) > ENVIROMENT_SIZE / 2) {
			obj.position.x = copysignf(ENVIROMENT_SIZE / 2, obj.position.x);
		}
		if (fabs(obj.position.y) > ENVIROMENT_SIZE / 2) {
			obj.position.y = copysignf(ENVIROMENT_SIZE / 2, obj.position.y);
		}
	}

	// Update position and speed of objects
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

	// Calculate forces between objects
	for (auto& obj : this->objects) {
		obj.forcesVector.clear();
		for (auto& grav : this->objects) {
			if (!(grav == obj)) {
				obj.forcesVector.push_back(this->calcGravityForce(obj, grav));
			}
		}
	}

	// Draw axes
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

		signed char offsetX = (drawX < windowSize.y - this->viewY)
								  ? 8
								  : -8 - ImGui::GetFontSize(),
					offsetY = (drawY < windowSize.x - this->viewX)
								  ? 5
								  : -5 - ImGui::CalcTextSize("-0.0e+00m").x;

		// Draw x axis labels
		for (int x = -this->viewX / length - 1;
			 x <= (-this->viewX + windowSize.x) / length + 2; x++) {
			char label[12];
			std::snprintf(&label[0], 12, "%.1em", x * step);
			ImVec2 point(p0.x + x * length + this->viewX, p0.y + drawX);
			list->AddLine(ImVec2(point.x, point.y - 5),
						  ImVec2(point.x, point.y + 5), this->axesColor);
			list->AddText(ImVec2(point.x - 40, point.y + offsetX),
						  this->axesStepsColor, &label[0]);
		}

		// Draw y axis labels
		for (int y = -this->viewY / length - 1;
			 y <= (-this->viewY + windowSize.y) / length + 2; y++) {
			if (y == 0) continue;
			char label[12];
			std::snprintf(&label[0], 12, "%.1em", y * step);
			ImVec2 point(p0.x + drawY, p0.y + y * length + this->viewY);
			list->AddLine(ImVec2(point.x - 5, point.y),
						  ImVec2(point.x + 5, point.y), this->axesColor);
			list->AddText(ImVec2(point.x + offsetY, point.y - 8),
						  this->axesStepsColor, &label[0]);
		}
	}

	list->PushClipRect(p0, ImVec2(p0.x + windowSize.x, p0.y + windowSize.y));
	ImGui::End();
}

void Gravity::reset() {
	this->objects.clear();
	object object1, object2;

	object1.mass = EARTH_MASS;
	object1.position = ImVec2(0, 0);
	object1.radius = EARTH_RADIUS;

	object2.mass = 1;
	object2.position = ImVec2(0, EARTH_RADIUS * 2);
	object2.radius = EARTH_RADIUS * 0.2;
	object2.move.speedX =  // Orbital Speed for object1
		std::sqrt(GRAVITY_G * object1.mass / (EARTH_RADIUS * 2));

	this->objects.push_back(object1);
	this->objects.push_back(object2);
}

void Gravity::object::simplify() {
	Force force = resultantOfForces(this->forcesVector);
	this->forcesVector.clear();
	this->forcesVector.push_back(force);
}

bool Gravity::object::operator==(const object& obj) {
	return (obj.position.x == this->position.x &&
			obj.position.y == this->position.y && obj.radius == this->radius &&
			obj.mass == this->mass && obj.move.speedX == this->move.speedX &&
			obj.move.speedY == this->move.speedY);
}

Force Gravity::calcGravityForce(const Gravity::object& o1,
								const Gravity::object& o2) {
	Force force;
	force.power = GRAVITY_G * o1.mass * o2.mass /
				  (std::pow(o1.position.x - o2.position.x, 2) +
				   std::pow(o1.position.y - o2.position.y, 2));
	force.angle = angleBetweenPoints(o1.position, o2.position);
	return force;
}

void Gravity::editObjectMenu(float& mass, float& radius, float& speedX,
							 float& speedY) {
	// TODO: Incress accuranct
	ImGui::SliderFloat(
		tr("Mass").c_str(), &mass, 0.001f, 1e30f, "%.4e kg",
		ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat(
		tr("Radius").c_str(), &radius, 0.001f, 1e7, "%.3f m",
		ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat(
		(tr("Speed") + " X").c_str(), &speedX, -LIGHT_SPEED, LIGHT_SPEED,
		"%.5f m/s",
		ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
	ImGui::SliderFloat(
		(tr("Speed") + " Y").c_str(), &speedY, -LIGHT_SPEED, LIGHT_SPEED,
		"%.5f m/s",
		ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
	ImGui::Text((tr("Total speed") + ": % .2f m/s").c_str(),
				std::sqrt(std::pow(speedX, 2) + std::pow(speedY, 2)));
}