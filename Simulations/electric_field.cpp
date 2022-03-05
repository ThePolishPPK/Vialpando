#include "electric_field.hpp"

#include <imgui.h>

#include "../translate.hpp"

ElectricField::ElectricField() {
	this->name = "Electric field";
	this->keepActive = true;
}

void ElectricField::draw() {
	ImGui::Begin(tr(name).c_str(), &keepActive);
	// Menu
	ImGui::Text(tr("Available simulations").c_str());
	ImGui::Checkbox(tr("Electric charge field needle").c_str(),
					&isElectroMagneticNeedlesActive);

	ImGui::End();

	if (isElectroMagneticNeedlesActive) drawElectroMagneticNeedles();
}