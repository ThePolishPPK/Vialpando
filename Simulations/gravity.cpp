#include "gravity.hpp"

#include <imgui.h>
#include <stdio.h>

Gravity::Gravity() {
	this->keepActive = false;
	this->name = "Gravity";
}

void Gravity::draw() {
	ImGui::Begin(this->name, &this->keepActive);
	ImGui::Text("Gravity .....");
	ImGui::End();
}