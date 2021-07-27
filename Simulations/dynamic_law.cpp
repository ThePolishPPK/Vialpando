#include "dynamic_law.hpp"

#include <imgui.h>

Dynamics::Dynamics() { this->name = "Dynamic Laws"; this->keepActive = true;}

void Dynamics::draw() {
	// 1 Deskorolka
	// 2 Równia pochyła
	// 3 Armata odpychająca kulę

	if (ImGui::Begin(this->name, &this->keepActive, ImGuiWindowFlags_NoResize)) {
		ImGui::Checkbox("Skateboard", &this->skateActive);
		ImGui::Checkbox("InclinedPlane", &this->inclinedPlaneActive);
		ImGui::Checkbox("Cannon", &this->cannonActive);
		ImGui::End();
	}

	if (this->skateActive) {
		ImGui::Begin("Dynamic Laws - Skateboard", &this->skateActive);
		ImGui::End();
	}

	if (this->inclinedPlaneActive) {
		ImGui::Begin("Dynamic Laws - Inclined Plane", &this->inclinedPlaneActive);
		ImGui::End();	
	}

	if (this->cannonActive) {
		ImGui::Begin("Dynamic Laws - Cannon", &this->cannonActive);
		ImGui::End();	
	}
}

void Dynamics::drawSkateboard() {

}

void Dynamics::drawInclinedPlane() {

}

void Dynamics::drawCannon() {

}