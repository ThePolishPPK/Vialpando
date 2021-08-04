#include <imgui.h>
#include <cmath>
#include <cstdio>	// For build mode only
#include "../translate.hpp"

#include "work_and_energy.hpp"

WorkAndEnergy::WorkAndEnergy() {
	this->name = "Work and energy";
	this->keepActive = true;
}

void WorkAndEnergy::draw() {
	ImGui::Begin(tr(this->name).c_str(), &this->keepActive);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 wSize = ImGui::GetWindowSize(), wPos = ImGui::GetWindowPos(), baseCursorPos = ImGui::GetCursorScreenPos();

	// Draw spring
	ImVec2 coilDiff(this->position/this->coilsCount, 0);
	coilDiff.y = std::sqrt(std::pow(this->coilLength, 2) - std::pow(coilDiff.x, 2))*this->scale;
	coilDiff.x *= this->scale;

	ImVec2 coils[this->coilsCount+1];
	coils[0] = ImVec2(wPos.x, wPos.y+wSize.y/2);

	for (unsigned short coil = 0; coil < this->coilsCount; coil++) {
		coils[coil+1] = ImVec2(coils[coil].x+coilDiff.x, coils[coil].y+coilDiff.y);
		coilDiff.y *= -1;
	}
	this->pkg.x = coils[this->coilsCount].x - wPos.x;
	this->pkg.y = coils[this->coilsCount].y - wPos.y - (this->pkg.height*this->scale-std::abs(coilDiff.y))/2;

	draw->AddPolyline(&coils[0], this->coilsCount+1, ImColor(255,0,0), ImDrawFlags_RoundCornersAll, 3);

	// Draw package
	ImVec2 pkgWindowPos(this->pkg.x, this->pkg.y), pkgSize(this->pkg.width*this->scale, this->pkg.height*this->scale);
	ImGui::SetCursorPos(pkgWindowPos);
	ImGui::InvisibleButton("", pkgSize);
	static bool pkgMove = false;
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		if (ImGui::IsItemHovered()) {
			pkgMove = true;
		}
		if (pkgMove) {
			this->position += io.MouseDelta.x/this->scale;
			this->speed = 0;
		}
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		pkgMove = false;
	}
	draw->AddRect(ImVec2(pkgWindowPos.x+wPos.x, pkgWindowPos.y+wPos.y), ImVec2(pkgWindowPos.x+wPos.x+pkgSize.x, pkgWindowPos.y+wPos.y+pkgSize.y), ImColor(0, 255, 0));

	// Calculate force, energy and displacement of spring.
	float displacement = this->position-this->baseLength,
		force = -displacement*this->k,
		work = std::pow(displacement, 2)*this->k/2,
		acceleration = force/this->pkg.mass;

	// FIXME: Simulation lose precision. Spring on start have x=20, after end 1 cycle x=0. Inaccuracy
	float delta = ImGui::GetTime() - this->lastUpdate;
	if (delta <= 1) {
		this->position += this->speed*delta;
		this->speed += acceleration*delta;
		if (this->position < 0 || this->position > this->coilLength*this->coilsCount) {
			this->speed = 0;
			this->position = (this->position < 0)? 0 : this->coilLength*this->coilsCount;
		}
	}
	this->lastUpdate = ImGui::GetTime();


	draw->PushClipRectFullScreen();
	ImGui::End();
}