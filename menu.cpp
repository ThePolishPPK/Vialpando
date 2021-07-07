#include "menu.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <algorithm>
#include <map>
#include <string>

#include "Simulations/gravity.hpp"

Menu::Menu(ImGuiIO& io) {
	this->IO = &io;
	this->keepActive = true;
	this->views.push_back(new Gravity());
}

void Menu::draw() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowBgAlpha(1);
	ImGui::Begin("Menu", NULL,
				 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
					 ImGuiWindowFlags_NoResize |
					 ImGuiWindowFlags_NoSavedSettings |
					 ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::Text("Dostępne symulacje");
	for (auto& view : this->views) {
		if (ImGui::Button(Menu::translation[std::string(view->name)]) &&
			find(activeSimulations.begin(), activeSimulations.end(), view) ==
				activeSimulations.end()) {
			this->activeSimulations.push_back(view);
			view->keepActive = true;
		}
	}
	ImGui::End();
	for (auto& view : this->activeSimulations) {
		if (view->keepActive == false) {
			this->activeSimulations.erase(
				find(activeSimulations.begin(), activeSimulations.end(), view));
		} else
			view->draw();
	}
}

std::map<std::string, const char*> Menu::translation({
	{std::string("Gravity"), "Siła grawitacji"},

});