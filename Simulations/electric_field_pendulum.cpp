#include <../basic.hpp>
#include <../translate.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

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
		obj1.neutralPosition = {0.2f, 0.3f};
		obj1.mass = 1.0f;
		obj1.charge = 4e-6f;
		obj1.radius = 0.04f;

		ballObject obj2;
		obj2.neutralPosition = {0.602f, 0.273f};
		obj2.mass = 1.7f;
		obj2.charge = -5e-6f;
		obj2.radius = 0.047f;

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
	if (ImGui::GetWindowSize().x < 10) ImGui::SetWindowSize({800, 500});
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImDrawList* drawL = ImGui::GetWindowDrawList();
	ImVec2 mousePtr = ImGui::GetMousePos();
	mousePtr.x -= windowPos.x;
	mousePtr.x /= scaleX;
	mousePtr.y -= windowPos.y;
	mousePtr.y /= scaleY;

	// Objects menu
	static ballObject* menuObject = NULL;
	if (ImGui::BeginPopupModal(
			"ModifyObject", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		if (menuObject == NULL) {
			ballObject newObject;
			newObject.neutralPosition = mousePtr;
			objects.push_back(newObject);
			menuObject = &objects.back();
		}
		ImGui::SetWindowSize({300, 120});
		ImGui::Text((tr("Charge") + ":").c_str());
		ImGui::InputFloat("", &(menuObject->charge), 1e-6f, 1.0f, "%.3e C");
		if (ImGui::Button(tr("Remove").c_str())) {
			auto iter = find(objects.begin(), objects.end(), *menuObject);
			objects.erase(iter);
			menuObject = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(tr("Close").c_str())) {
			menuObject = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
		for (auto& obj : objects) {
			if (distanceBetweenPoints(obj.position, mousePtr) <= obj.radius) {
				menuObject = &obj;
			}
		}
		ImGui::OpenPopup("ModifyObject");
	}

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

			// Discharge and pendium reset
			if (distance <= obj1.radius + obj2.radius) {
				obj1.charge += obj2.charge;
				obj1.charge /= 2;
				obj2.charge = obj1.charge;

				float pendium = obj1.mass * obj1.move.x + obj2.mass * obj2.move.x;
				pendium /= 2;
				obj1.move.x = pendium / obj1.mass;
				obj2.move.x = pendium / obj2.mass;
			}

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
	}

	// Calculate displacement
	float timeDelta = ImGui::GetTime() - lastUpdate;
	for (auto& obj : objects) {
		auto& force = obj.forces.back();
		obj.move.x += - force.power/obj.mass * cos(force.angle) * timeDelta;
		obj.position.x -= obj.neutralPosition.x;
		obj.position.x += obj.move.x * cos(obj.angleOfLine) * timeDelta;
		obj.position.y = pow(obj.neutralPosition.y, 2) - pow(obj.position.x, 2);
		if (obj.position.y <= 0 || abs(obj.position.x) > obj.neutralPosition.y) {
			obj.position.x = (signbit(obj.position.x) ? -1 : 1) * obj.neutralPosition.y;
			obj.position.y = 1e-10;
			obj.move.x = 0;
		} else obj.position.y = sqrt(obj.position.y);
		obj.angleOfLine = atan2(obj.position.x, obj.position.y);
		
		obj.forces.clear();
	}
	lastUpdate += timeDelta;


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