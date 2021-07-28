#include "dynamic_law.hpp"

#include <imgui.h>

#include "../translate.hpp"

Dynamics::Dynamics() {
	this->name = "Dynamic Laws";
	this->keepActive = true;
}

void Dynamics::draw() {
	// 1 Deskorolka
	// 2 Równia pochyła
	// 3 Armata odpychająca kulę

	if (ImGui::Begin(tr(this->name).c_str(), &this->keepActive,
					 ImGuiWindowFlags_NoResize)) {
		ImGui::Checkbox(tr("Skateboard").c_str(), &this->skateActive);
		ImGui::Checkbox(tr("Inclined plane").c_str(),
						&this->inclinedPlaneActive);
		ImGui::Checkbox(tr("Cannon").c_str(), &this->cannonActive);
		ImGui::End();
	}

	if (this->skateActive) {
		ImGui::Begin((tr("Dynamic Laws") + " - " + tr("Skateboard")).c_str(),
					 &this->skateActive);
		ImGui::End();
	}

	if (this->inclinedPlaneActive) {
		ImGui::Begin(
			(tr("Dynamic Laws") + " - " + tr("Inclined plane")).c_str(),
			&this->inclinedPlaneActive);
		drawInclinedPlane();
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Mass").c_str(), &this->i.mass, 1.0f, 0.01f,
						 std::pow(2, 16), "%.2f kg",
						 ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Gravity").c_str(), &this->i.gravity, 0.1f, 0.1f,
						 std::pow(2, 8), "%.1f m/s²",
						 ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Angle").c_str(), &this->i.inclination, 0.1f, 0.0f,
						 90.0f, "%.1f°", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Friction ratio").c_str(), &this->i.frictionRatio,
						 0.01f, 0.0f, 100.0f, "%.2f",
						 ImGuiSliderFlags_AlwaysClamp);

		this->i.gravityForce = this->i.gravity * this->i.mass;
		this->i.slipForce =
			std::cos((this->i.inclination / 180) * M_PI) * this->i.gravityForce;
		this->i.pressureForce =
			std::sin((this->i.inclination / 180) * M_PI) * this->i.gravityForce;
		this->i.frictionForce = this->i.frictionRatio * this->i.pressureForce;
		if (this->i.frictionForce > this->i.slipForce)
			this->i.frictionForce = this->i.slipForce;

		ImGui::End();
	}

	if (this->cannonActive) {
		ImGui::Begin((tr("Dynamic Laws") + " - " + tr("Cannon")).c_str(),
					 &this->cannonActive);
		ImGui::End();
	}
}

void Dynamics::drawSkateboard() {}

void Dynamics::drawInclinedPlane() {
	ImDrawList* draw = ImGui::GetWindowDrawList();

	ImVec2 wPos = ImGui::GetWindowPos();
	ImVec2 wSize = ImGui::GetWindowSize();

	ImVec2 p1(wPos.x + 10, wPos.y + wSize.y - 10),
		p2(wPos.x + wSize.x - 10, p1.y);
	ImVec2 p3(p1.x,
			  p1.y - this->heightOnSlope(this->i.inclination) * (p2.x - p1.x));

	draw->AddTriangleFilled(p1, p2, p3, ImColor(0, 64, 255));  // Slope

	float slopeRate = std::tan((this->i.inclination / 180) * M_PI),
		  w = wSize.x - (2 * 10), pos = 1 - this->i.position;

	ImVec2 q1(p1.x + (pos * w), p1.y - this->i.position * w * slopeRate);
	ImVec2 q2(p1.x + ((pos + 0.1) * w),
			  p1.y - (this->i.position - 0.1) * w * slopeRate),
		q3(q2.x + slopeRate * 50, q2.y - 50),
		q4(q1.x + slopeRate * 50, q1.y - 50);

	draw->AddQuadFilled(q1, q2, q3, q4, ImColor(255, 0, 0));  // Square on slope

	draw->PushClipRect(wPos, ImVec2(wPos.x + wSize.x, wPos.y + wSize.y));
}

void Dynamics::drawCannon() {}

float Dynamics::heightOnSlope(float angle, float pos) {
	return std::tan((angle / 180) * M_PI) * pos;
}