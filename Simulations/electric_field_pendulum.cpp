#include <../basic.hpp>
#include <../translate.hpp>
#include <cmath>
#include <vector>

#include "electric_field.hpp"

using namespace std;

struct ballObject : object {
	ballObject(){};
	ImVec2 neutralPosition = {
		0, 0};	// Default position of ball when additional forces doesn't work.
	float angleOfLine = 0.0f;  // Angle of line maked with neutral position
	float radius = 0.05f;	   // Radius of ball. In meters.
};

void ElectricField::drawElectroMagneticPendulum() {
	static float scaleX = 500.0f;	 // ScaleX px = 1m
	static float scaleY = 500.0f;	 // ScaleY px = 1m
	static float scaleForce = 4.0f;	 // ScaleForce px = 1N
	static vector<ballObject> objects;
	static bool _oneTimeInitializationProcess = []() {
		ballObject obj1;
		obj1.neutralPosition = {0.2f, 0.4f};
		obj1.mass = 1.0f;
		obj1.charge = 3e-6f;
		obj1.radius = 0.04f;

		ballObject obj2;
		obj2.neutralPosition = {0.4f, 0.4f};
		obj2.mass = 1.7f;
		obj2.charge = -9e-6f;
		obj2.radius = 0.047f;

		// FIXME: To remove – For tests only
		obj1.angleOfLine = M_PI / 6;
		obj2.angleOfLine = -M_PI / 6;

		objects.push_back(obj1);
		objects.push_back(obj2);
		return true;
	}();
	static float gravity = 9.81;  // Meters per squere second
	static float lastUpdate = ImGui::GetTime();

	ImGui::Begin(tr("Electric pendulum").c_str(),
				 &this->isElectroMagneticPendulumActive,
				 ImGuiWindowFlags_MenuBar);

	// Get window info
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImDrawList* drawL = ImGui::GetWindowDrawList();

	// Get objects position
	for (auto& obj : objects) {
		obj.position.x = obj.neutralPosition.x +
						 obj.neutralPosition.y * sin(obj.angleOfLine);
		obj.position.y = obj.neutralPosition.y +
						 obj.neutralPosition.y * cos(obj.angleOfLine);
	}

	// Calculate forces
	for (uint8_t o1 = 0; o1 < objects.size(); o1++) {
		auto& obj1 = objects[o1];
		for (uint8_t o2 = o1 + 1; o2 < objects.size(); o2++) {
			auto& obj2 = objects[o2];

			float distance =
				distanceBetweenPoints(obj1.position, obj2.position);
			float force =
				this->k * obj1.charge * obj2.charge / pow(distance, 2);
			float angle = atan((obj2.position.y - obj1.position.y) /
							   (obj2.position.x - obj1.position.x));

			if (obj1.position.x > obj2.position.x) {
				angle = -1 / angle;
			}

			obj1.forces.push_back(Force(force, angle));
			obj2.forces.push_back(Force(force, angle+M_PI));
		}
		// Gravity as two parts
		obj1.forces.push_back(
			Force(obj1.mass * gravity * cos(obj1.angleOfLine), -M_PI / 2));
		obj1.forces.push_back(
			Force(obj1.mass * gravity * sin(obj1.angleOfLine), 0));
	}

	// Draw lines
	for (auto& obj : objects) {
		drawL->AddLine(
			ImVec2(windowPos.x + obj.neutralPosition.x * scaleX, windowPos.y),
			ImVec2(windowPos.x + (obj.position.x * scaleX),
				   windowPos.y + (obj.position.y * scaleY)),
			ImColor(80, 0, 200), 3.0f);
	}

	// Draw objects
	for (auto& obj : objects) {
		drawL->AddCircleFilled(
			ImVec2(windowPos.x + (obj.position.x * scaleX),
				   windowPos.y + ceil(obj.position.y * scaleY)),
			obj.radius * scaleX, ImColor(70, 200, 40));
	}

	// Draw forces and make resultants
	for (auto& obj : objects) {
		ImVec2 basePos = ImVec2(windowPos.x + (obj.position.x * scaleX),
								windowPos.y + ceil(obj.position.y * scaleY));
		for (auto& f : obj.forces) {
			drawArrow(basePos,
					  ImVec2(basePos.x - f.power * cos(f.angle) * scaleForce,
							 basePos.y - f.power * sin(f.angle) * scaleForce),
					  drawL);
		}
		obj.forces = {resultantOfForces(obj.forces)};
		obj.forces.clear();
	}

	// Calculate displacement
	// TODO: Add segment of calculating displacement and move

	// TODO: Add Menu to manage properties of balls

	// TODO: Add mouse button functionality – Adding objects and faster access
	// to edit menu for specifed objects

	// Optional TODO: Discharge balls when touch – distance between centers
	// lower than sum of radiuses

	// Menu
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(tr("Options").c_str())) {
			ImGui::DragFloat(tr("Force vector scale").c_str(), &scaleForce,
							 0.1f, 0, 0);
			ImGui::DragFloat((tr("Scale") + " X").c_str(), &scaleX, 1.0f, 10.0f,
							 1000000.0f);
			ImGui::DragFloat((tr("Scale") + " Y").c_str(), &scaleY, 1.0f, 10.0f,
							 1000000.0f);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	drawL->PushClipRectFullScreen();
	ImGui::End();
}