#include "menu.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <algorithm>
#include <map>
#include <string>

#include "Simulations/dynamic_law.hpp"
#include "Simulations/gravity.hpp"
#include "translate.hpp"

Menu::Menu(ImGuiIO& io) {
	this->IO = &io;
	this->keepActive = true;
	this->views.push_back(new Gravity());
	this->views.push_back(new Dynamics());
	processDicts();
	setLanguage(std::string("pl"));
}

void Menu::draw() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowBgAlpha(1);
	ImGui::Begin(tr("Menu").c_str(), NULL,
				 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
					 ImGuiWindowFlags_NoResize |
					 ImGuiWindowFlags_NoSavedSettings |
					 ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::Text("%s", tr("Available simulations").c_str());
	for (auto& view : this->views) {
		if (ImGui::Button(tr(view->name).c_str()) &&
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