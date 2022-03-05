#ifndef ELECTRIC_FIELD_H
#define ELECTRIC_FIELD_H

#include <imgui.h>

#include <vector>

#include "../basic.hpp"
#include "../view.hpp"

class ElectricField : public View {
   public:
	ElectricField();
	void draw();
	const float k = 8.99e9;

   private:
	ImGuiIO &io = ImGui::GetIO();

	void drawElectroMagneticNeedles();
	bool isElectroMagneticNeedlesActive = false;

	std::vector<Force> calcNeedleForces(const ImVec2 &location,
										std::vector<object> &objects);
};

#endif