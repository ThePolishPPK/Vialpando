#include "dynamic_law.hpp"

#include <imgui.h>

#include "../basic.hpp"
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
			&this->inclinedPlaneActive,
			ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(tr("Options").c_str())) {
				ImGui::SetNextItemWidth(128);
				ImGui::DragFloat(tr("Force vector scale").c_str(),
								 &this->i.forceScale, 0.5f, 0.001f,
								 std::pow(2, 12), "%.2f : 1",
								 ImGuiSliderFlags_AlwaysClamp |
									 ImGuiSliderFlags_Logarithmic);
				ImGui::SetNextItemWidth(128);
				ImGui::DragFloat(tr("Time speed").c_str(), &this->i.timeScale,
								 0.5f, 0.001f, std::pow(2, 12), "%.2f : 1",
								 ImGuiSliderFlags_AlwaysClamp |
									 ImGuiSliderFlags_Logarithmic);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		drawInclinedPlane();
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(
			tr("Mass").c_str(), &this->i.mass, 5.0f, 0.01f, std::pow(2, 16),
			"%.2f kg",
			ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Gravity").c_str(), &this->i.gravity, 0.1f, 0.1f,
						 std::pow(2, 8), "%.1f m/s²",
						 ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Angle").c_str(), &this->i.inclination, 0.1f, 0.0f,
						 90.0f, "%.1f°", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Friction ratio").c_str(), &this->i.frictionRatio,
						 0.004f, 0.0f, 100.0f, "%.2f",
						 ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(
			tr("Distance").c_str(), &this->i.distance, 0.5f, 0.001f,
			std::pow(2, 12), "%.2f m",
			ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
		if (ImGui::Button(tr("Move up").c_str())) {
			this->i.position = 1.0f;
		}

		this->i.gravityForce = this->i.gravity * this->i.mass;
		this->i.slipForce =
			std::sin((this->i.inclination / 180) * M_PI) * this->i.gravityForce;
		this->i.pressureForce =
			std::cos((this->i.inclination / 180) * M_PI) * this->i.gravityForce;
		this->i.frictionForce = this->i.frictionRatio * this->i.pressureForce;
		if (this->i.frictionForce > this->i.slipForce && this->i.speed <= 0)
			this->i.frictionForce = this->i.slipForce;

		float timeDelta = ImGui::GetTime() - this->i.lastRefresh,
			  part = timeDelta / this->i.timeScale;
		if (timeDelta <= 1) {
			if (this->i.position > 0) {
				this->i.position -= this->i.speed * part / this->i.distance;
				this->i.speed += (this->i.slipForce - this->i.frictionForce) *
								 part / this->i.mass;
				if (this->i.speed < 0) this->i.speed = 0;
			} else {
				this->i.speed = 0;
			}
		}
		this->i.lastRefresh = ImGui::GetTime();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 200, 60));

		ImGui::Text((tr("Speed") + ": %.2fm/s\n" + tr("Gravity force") +
					 ": %.1fN\n" + tr("Friction force") + ": %.1fN\n" +
					 tr("Slip force") + ": %.1fN\n" + tr("Pressure force") +
					 ": %.1fN\n" + tr("Ground reaction") + ": %.1fN\n")
						.c_str(),
					this->i.speed, this->i.gravityForce, this->i.frictionForce,
					this->i.slipForce, this->i.pressureForce,
					this->i.pressureForce);

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::InvisibleButton("#inclinedPlane", ImGui::GetWindowSize());
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
			ImGui::IsItemFocused() && ImGui::IsItemActive()) {
			ImVec2 delta = ImGui::GetIO().MouseDelta;
			this->i.viewX += delta.x;
			this->i.viewY += delta.y;
		}

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

	if (ImGui::GetIO().MouseWheel != 0) {
		this->i.scale *= 1 - (ImGui::GetIO().MouseWheel / 10);
	}

	ImVec2 p1(wPos.x + this->i.viewX,
			  wPos.y + (wSize.y / this->i.scale) + this->i.viewY),
		p2(p1.x + (wSize.x / this->i.scale), p1.y);
	ImVec2 p3(p1.x,
			  p1.y - this->heightOnSlope(this->i.inclination) * (p2.x - p1.x));

	draw->AddTriangleFilled(p1, p2, p3, ImColor(0, 64, 255));  // Slope

	float slopeAngle = (this->i.inclination / 180) * M_PI, w = p2.x - p1.x,
		  pos = 1 - this->i.position;

	ImVec2 q1(p1.x + (pos * w),
			  p1.y - this->i.position * w * std::tan(slopeAngle)),
		diff(std::cos(slopeAngle) * 50, std::sin(slopeAngle) * 50);
	ImVec2 q2(q1.x + diff.x, q1.y + diff.y), q3(q2.x + diff.y, q2.y - diff.x),
		q4(q1.x + diff.y, q1.y - diff.x);

	draw->AddQuadFilled(q1, q2, q3, q4, ImColor(255, 0, 0));  // Square on slope

	ImVec2 v0((q1.x + q3.x) / 2, (q1.y + q3.y) / 2),
		vg(v0.x, v0.y + (this->i.gravityForce / this->i.forceScale)),
		vf(q1.x - this->i.frictionForce * std::cos(slopeAngle) /
					  this->i.forceScale,
		   q1.y - this->i.frictionForce * std::sin(slopeAngle) /
					  this->i.forceScale),
		vs(v0.x + this->i.slipForce * std::cos(slopeAngle) / this->i.forceScale,
		   v0.y +
			   this->i.slipForce * std::sin(slopeAngle) / this->i.forceScale),
		vp(this->i.pressureForce * std::sin(slopeAngle) / this->i.forceScale,
		   this->i.pressureForce * std::cos(slopeAngle) / this->i.forceScale);

	draw->AddCircleFilled(v0, 3, ImColor(0, 255, 0));  // Center of many forces
	drawArrow(v0, vg, draw);						   // Gravity force vector
	drawArrow(q1, vf, draw);						   // Friction force vector
	drawArrow(v0, vs, draw);						   // Slip force vector
	drawArrow(v0, ImVec2(v0.x - vp.x, v0.y + vp.y),
			  draw);  // Pressure force vector
	drawArrow(v0, ImVec2(v0.x + vp.x, v0.y - vp.y),
			  draw);  // Ground reaction force vector

	draw->PushClipRect(wPos, ImVec2(wPos.x + wSize.x, wPos.y + wSize.y));
}

void Dynamics::drawCannon() {}

float Dynamics::heightOnSlope(float angle, float pos) {
	return std::tan((angle / 180) * M_PI) * pos;
}