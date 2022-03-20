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
	// FIXME: Search segmentation fault reason
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
	coils[0] = ImVec2(baseCursorPos.x, baseCursorPos.y + 20);

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

	// Update plot data
	static unsigned char pos = 0;
	static float plotDisplacement[256];
	static double lastPlotUpdate = ImGui::GetTime();
	if (ImGui::GetTime() - lastPlotUpdate >= 0.1) {
		plotDisplacement[pos] = this->position;
		pos++;
		lastPlotUpdate += 0.1;
	}

	// Draw force vector
	drawArrow(pkgCenterPoint,
			  ImVec2(pkgCenterPoint.x + force * this->scale, pkgCenterPoint.y),
			  draw);

	// Draw plot
	ImGui::SetCursorPos(ImVec2(0, 120 + this->coilLength * this->scale));

	ImGui::Text("%s:", tr("Displacement").c_str());
	ImGui::PlotLines("", &plotDisplacement[0], 256, 0, NULL, this->maxDX,
					 -this->maxDX, ImVec2(0, 200));

	int cursorY = ImGui::GetCursorPosY(), cursorX = 350;

	// Info elements
	ImGui::Text("%s:\t%f m/s\n%s:\t%f m/s²\n%s:\t%f N\n%s:\t%f m\n",
				tr("Speed").c_str(), speed, tr("Acceleration").c_str(),
				acceleration, tr("Force").c_str(), force,
				tr("Displacement").c_str(), this->position);

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

	ImGui::SetCursorPosX(cursorX);
	ImGui::SetNextItemWidth(100);
	ImGui::DragFloat(
		tr("Elasticity coefficient").c_str(), &this->k, 0.1, 0.01, 8, "%.3f",
		ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);

	// Catch block escape from system
	if (this->position > this->maxDXRight) {
		this->dx = this->maxDXRight;
		resetSystem();
	} else if (this->position < -this->maxDXLeft) {
		this->dx = this->maxDXLeft;
		resetSystem();
	}

	draw->PushClipRectFullScreen();
	ImGui::End();
}

void WorkAndEnergy::resetSystem() {
	double frequency = 1 / sqrt(this->pkg.mass / this->k);
	float angleSpeed = (2 * M_PI * frequency);

	this->position = std::signbit(this->position) ? -this->dx : this->dx;
	this->cycleStartTime = ImGui::GetTime();
	this->cycleStartTime -=
		(int)(this->cycleStartTime / angleSpeed) * angleSpeed;
	this->cycleStartTime +=
		(std::signbit(this->position) ? M_PI : -M_PI) / frequency / 2;
	this->maxDXLeft = this->baseLength;
	this->maxDXRight = this->coilLength * this->coilsCount - this->baseLength;
}