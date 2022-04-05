#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "../basic.hpp"
#include "../translate.hpp"
#include "electric_field.hpp"

void ElectricField::drawElectroMagneticNeedles() {
	static std::vector<object> objects = {
		object{{0.2f, 0.3f}, {0, 0}, 0, 0, 1.6e-3f},
		object{{0.8f, 0.3f}, {0, 0}, 0, 0, -1.6e-3f}};
	static float densityOfNeedles = 20.0f;	// Define net of needles length
	static float scale = 1000.0f;			// 1m = {scale} pixels
	static float objectSize = 0.05f;		// Size of object in meters
	static int arrowLength = 25;
	static object* lastMoved = NULL;
	static object* editObject = NULL;

	ImGui::Begin(tr("Electric charge field needle").c_str(),
				 &isElectroMagneticNeedlesActive, ImGuiWindowFlags_MenuBar);
	// TODO: Make auto minimum window scale.

	// Prepare window info
	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImVec2 windowSize = ImVec2(ImGui::GetWindowWidth() / scale,
							   ImGui::GetWindowHeight() / scale);
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 mousePtr = ImGui::GetMousePos();
	mousePtr.x -= windowPos.x;
	mousePtr.x /= scale;
	mousePtr.y -= windowPos.y;
	mousePtr.y /= scale;

	// Draw Needles
	static float maxCharge = fabs(objects[0].charge);
	float masterColorForce =
		(this->k / std::pow(objectSize * 1.5, 2)) * maxCharge;
	ImVec2 loc = ImVec2(0, 0);
	for (; loc.x < windowSize.x; loc.x += 1 / densityOfNeedles) {
		loc.y = 0;
		for (; loc.y < windowSize.y; loc.y += 1 / densityOfNeedles) {
			std::vector<Force> forces = this->calcNeedleForces(loc, objects);
			Force data = resultantOfForces(forces);
			float colorScale = sqrt(sqrt(data.power / masterColorForce));
			if (colorScale > 1.0f) colorScale = 1.0f;
			drawNeedle(
				ImVec2(windowPos.x + loc.x * scale,
					   windowPos.y + loc.y * scale),
				draw, arrowLength, data.angle / M_PI * 180,
				ImColor::HSV(colorScale, 1.0f, sqrt(colorScale))
			);
		}
		if (draw->IdxBuffer.size() > std::pow(2, 16) * 0.90) {
			densityOfNeedles *= 0.90;
		}
	}

	// Find maximum object charge
	maxCharge = 0.0f;
	for (auto& obj : objects)
		if (fabs(obj.charge) > maxCharge) maxCharge = fabs(obj.charge);

	// Draw objects
	for (auto& obj : objects) {
		draw->AddCircleFilled(ImVec2(windowPos.x + obj.position.x * scale,
									 windowPos.y + obj.position.y * scale),
							  objectSize * scale,
							  obj.charge > 0
								  ? ImColor(255, 0, 0)
								  : (obj.charge == 0 ? ImColor(255, 255, 255)
													 : ImColor(0, 0, 255)));
	}

	// Move objects
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && lastMoved == NULL) {
		for (auto& obj : objects) {
			if (distanceBetweenPoints(obj.position, mousePtr) <= objectSize) {
				lastMoved = &obj;
			}
		}
	}
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		if (lastMoved != NULL) {
			lastMoved->position.x += io.MouseDelta.x / scale;
			lastMoved->position.y += io.MouseDelta.y / scale;
		}
	} else {
		lastMoved = NULL;
	}

	// Popup Menu for editing objects
	if (ImGui::BeginPopupModal(
			"ModifyObject", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		if (editObject == NULL) {
			objects.push_back(object{mousePtr});
			editObject = &objects.back();
		}
		ImGui::SetWindowSize({300, 120});
		ImGui::Text("%s:", tr("Charge").c_str());
		ImGui::DragFloat("", &(editObject->charge), 4e-6f, -4.0f, 4.0f,
						 "%.6f C", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::Button(tr("Remove").c_str())) {
			auto iter = std::find(objects.begin(), objects.end(), *editObject);
			objects.erase(iter);
			editObject = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(tr("Close").c_str())) {
			editObject = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
		for (auto& obj : objects) {
			if (distanceBetweenPoints(obj.position, mousePtr) <= objectSize) {
				editObject = &obj;
			}
		}
		ImGui::OpenPopup("ModifyObject");
	}

	// Menu
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(tr("Options").c_str())) {
			ImGui::DragFloat(tr("Density of needles").c_str(),
							 &densityOfNeedles, 0.1f, 1.0f, 60.0f, "%.1f 1/m",
							 ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat(tr("Scale").c_str(), &scale, 1.0f, 10.0f,
							 1000000.0f, "%.0f m/px",
							 ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat(tr("Object size").c_str(), &objectSize, 0.01f,
							 0.04f, 5.0f, "%.2f m",
							 ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragInt(tr("Arrow size").c_str(), &arrowLength, 1, 3, 100,
						   "%d px", ImGuiSliderFlags_AlwaysClamp);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	draw->PushClipRectFullScreen();
	ImGui::End();
}

std::vector<Force> ElectricField::calcNeedleForces(
	const ImVec2& location, std::vector<object>& objects) {
	std::vector<Force> forces;
	for (auto& obj : objects) {
		Force objForce(
			(this->k /
			 std::pow(distanceBetweenPoints(obj.position, location), 2)) *
				obj.charge,
			angleBetweenPoints(location, obj.position));
		if (obj.charge == 0) {
			objForce.angle = 0;
		}  // Probablly useless feture
		forces.push_back(objForce);
	}
	return forces;
}