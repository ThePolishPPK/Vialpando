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
	ImVec2 coilDiff((this->position + this->baseLength) / this->coilsCount, 0);
	coilDiff.y =
		std::sqrt(std::pow(this->coilLength, 2) - std::pow(coilDiff.x, 2)) *
		this->scale;
	coilDiff.x *= this->scale;

	ImVec2 coils[this->coilsCount + 1];
	// FIXME: Repair coil positions coordinates generator, to keep layout
	// structure.
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
			this->dx = this->position * (std::signbit(this->position) ? -1 : 1);
		}
		if (pkgMove) {
			this->dx += (std::signbit(this->position) ? -1 : 1) *
						io.MouseDelta.x / this->scale;
			this->position = (std::signbit(this->position) ? -1 : 1) * this->dx;
			this->dx *= std::signbit(this->dx) ? -1 : 1;
		}
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		pkgMove = false;
		resetSystem();
	}
	draw->AddRect(ImVec2(pkgWindowPos.x + wPos.x, pkgWindowPos.y + wPos.y),
				  ImVec2(pkgWindowPos.x + wPos.x + pkgSize.x,
						 pkgWindowPos.y + wPos.y + pkgSize.y),
				  ImColor(0, 255, 0));

	// Master calculations
	float force, speed, acceleration;
	if (pkgMove) {
		force = 0;
		speed = 0;
		acceleration = 0;
	} else {
		double timeCycleDelta = ImGui::GetTime() - this->cycleStartTime;
		double frequency = 1 / sqrt(this->pkg.mass / this->k);
		this->position = this->dx * sin(frequency * timeCycleDelta);
		acceleration = -this->dx * std::pow(frequency, 2) *
					   sin(frequency * timeCycleDelta);
		force = acceleration * this->pkg.mass;
		speed = this->dx * frequency * sin(frequency * timeCycleDelta);
	}

	// Draw plot
	static unsigned char pos = 0;
	static float plotDisplacement[256];
	static double lastPlotUpdate = ImGui::GetTime();
	if (ImGui::GetTime() - lastPlotUpdate >= 0.1) {
		plotDisplacement[pos] = this->position;
		pos++;
		lastPlotUpdate += 0.1;
	}

	// Info elements
	drawArrow(pkgCenterPoint,
			  ImVec2(pkgCenterPoint.x + force * this->scale, pkgCenterPoint.y),
			  draw);

	ImGui::SetCursorPos(ImVec2(0, coils[0].y + this->coilLength));

	ImGui::PlotLines(tr("Displacement").c_str(), &plotDisplacement[0], 256, 0,
					 NULL, this->maxDX, -this->maxDX, ImVec2(0, 200));

	int cursorY = ImGui::GetCursorPosY(), cursorX = 350;

	ImGui::Text(
		(tr("Speed") + ":\t%f m/s\n" + tr("Acceleration") + ":\t%f m/s²\n" +
		 tr("Force") + ":\t%f N\n" + tr("Displacement") + ":\t%f m\n")
			.c_str(),
		speed, acceleration, force, this->position);

	// Settings widgets
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

	// Catch block escape from system
	if (fabsf(this->position) > this->maxDX) {
		this->dx = this->maxDX;
		resetSystem();
	}

	draw->PushClipRectFullScreen();
	ImGui::End();
}

void WorkAndEnergy::resetSystem() {
	// TODO: Refactor to reduce code and operations!
	this->position = std::signbit(this->position) ? -this->dx : this->dx;
	double frequency = 1 / sqrt(this->pkg.mass / this->k);
	this->cycleStartTime = ImGui::GetTime();
	this->cycleStartTime -=
		(int)(this->cycleStartTime / (2 * M_PI * frequency)) *
		(2 * M_PI * frequency);
	this->cycleStartTime += std::signbit(this->position)
								? M_PI / frequency / 2
								: -M_PI / frequency / 2;
}