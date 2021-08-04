#include "work_and_energy.hpp"

#include <imgui.h>

#include <cmath>
#include <cstdio>  // For build mode only

#include "../basic.hpp"
#include "../translate.hpp"

WorkAndEnergy::WorkAndEnergy() {
	this->name = "Work and energy";
	this->keepActive = true;
}

void WorkAndEnergy::draw() {
	ImGui::Begin(tr(this->name).c_str(), &this->keepActive);

	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 wSize = ImGui::GetWindowSize(), wPos = ImGui::GetWindowPos(),
		   baseCursorPos = ImGui::GetCursorScreenPos();

	// Draw spring
	ImVec2 coilDiff(this->position / this->coilsCount, 0);
	coilDiff.y =
		std::sqrt(std::pow(this->coilLength, 2) - std::pow(coilDiff.x, 2)) *
		this->scale;
	coilDiff.x *= this->scale;

	ImVec2 coils[this->coilsCount + 1];
	coils[0] = ImVec2(wPos.x + baseCursorPos.x, wPos.y + baseCursorPos.y);

	for (unsigned short coil = 0; coil < this->coilsCount; coil++) {
		coils[coil + 1] =
			ImVec2(coils[coil].x + coilDiff.x, coils[coil].y + coilDiff.y);
		coilDiff.y *= -1;
	}
	this->pkg.x = coils[this->coilsCount].x - wPos.x;
	this->pkg.y = -(coilDiff.y > 0 ? 0 : -coilDiff.y) +
				  coils[this->coilsCount].y - wPos.y -
				  (this->pkg.height * this->scale - std::abs(coilDiff.y)) / 2;

	draw->AddPolyline(&coils[0], this->coilsCount + 1, ImColor(255, 0, 0),
					  ImDrawFlags_RoundCornersAll, 3);

	// Draw package
	ImVec2 pkgWindowPos(this->pkg.x, this->pkg.y),
		pkgSize(this->pkg.width * this->scale, this->pkg.height * this->scale),
		pkgCenterPoint(pkgWindowPos.x + pkgSize.x / 2 + wPos.x,
					   pkgWindowPos.y + pkgSize.y / 2 + wPos.y);
	ImGui::SetCursorPos(pkgWindowPos);
	ImGui::InvisibleButton("", pkgSize);
	static bool pkgMove = false;
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		if (ImGui::IsItemHovered()) {
			pkgMove = true;
		}
		if (pkgMove) {
			this->position += io.MouseDelta.x / this->scale;
			this->speed = 0;
		}
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		pkgMove = false;
	}
	draw->AddRect(ImVec2(pkgWindowPos.x + wPos.x, pkgWindowPos.y + wPos.y),
				  ImVec2(pkgWindowPos.x + wPos.x + pkgSize.x,
						 pkgWindowPos.y + wPos.y + pkgSize.y),
				  ImColor(0, 255, 0));

	// Calculate force, energy and displacement of spring.
	float displacement = this->position - this->baseLength,
		  force = -displacement * this->k,
		  work = std::pow(displacement, 2) * this->k / 2,
		  acceleration = force / this->pkg.mass;

	// FIXME: Simulation lose precision. Spring on start have x=20, after end 1
	// cycle x=0. Inaccuracy
	float delta = ImGui::GetTime() - this->lastUpdate;
	if (delta <= 1) {
		this->position += this->speed * delta;
		this->speed += acceleration * delta;
	}
	this->lastUpdate = ImGui::GetTime();

	static unsigned char pos = 0;
	static float plotDisplacement[256];
	static double lastPlotUpdate;
	if (this->lastUpdate - lastPlotUpdate >= 0.1) {
		plotDisplacement[pos] = displacement;
		pos++;
		lastPlotUpdate += 0.1;
	}

	// Info elements
	drawArrow(pkgCenterPoint,
			  ImVec2(pkgCenterPoint.x + force * this->scale, pkgCenterPoint.y),
			  draw);

	ImGui::SetCursorPos(ImVec2(0, coils[0].y + this->coilLength));

	ImGui::PlotLines(tr("Displacement").c_str(), &plotDisplacement[0], 256, 0,
					 NULL, -this->baseLength,
					 this->coilLength * this->coilsCount - this->baseLength,
					 ImVec2(0, 200));

	int cursorY = ImGui::GetCursorPosY(), cursorX = 350;

	ImGui::Text((tr("Speed") + ":\t%f m/s\n" + tr("Acceleration") +
				 ":\t%f m/s²\n" + tr("Force") + ":\t%f N\n" + tr("Work") +
				 ":\t%f J\n" + tr("Displacement") + ":\t%f m\n")
					.c_str(),
				this->speed, acceleration, force, work, displacement);

	ImGui::SetCursorPos(ImVec2(350, cursorY));
	ImGui::SetNextItemWidth(100);
	ImGui::SliderInt(tr("Coils").c_str(), &this->coilsCount, 1, 256);
	ImGui::SetCursorPosX(cursorX);
	ImGui::SetNextItemWidth(100);
	ImGui::DragFloat(
		tr("Coils length").c_str(), &this->coilLength, 0.01, 0.001, 256,
		"%.3f m", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
	ImGui::SetCursorPosX(cursorX);
	ImGui::SetNextItemWidth(100);
	ImGui::DragFloat(
		tr("Mass").c_str(), &this->pkg.mass, 0.1, 0.01, 256, "%.2f kg",
		ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);
	ImGui::SetCursorPosX(cursorX);
	ImGui::SetNextItemWidth(100);
	ImGui::DragFloat(
		tr("Zero point").c_str(), &this->baseLength, 0.1, 0.01, 256, "%.2f m",
		ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);

	if (this->position < 0 ||
		this->position > this->coilLength * this->coilsCount) {
		this->speed = 0;
		this->position =
			(this->position < 0) ? 0 : this->coilLength * this->coilsCount;
	}

	draw->PushClipRectFullScreen();
	ImGui::End();
}