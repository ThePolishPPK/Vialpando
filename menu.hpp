#ifndef MENU_H
#define MENU_H

#include <imgui.h>

#include <map>
#include <string>
#include <vector>

#include "translate.hpp"
#include "view.hpp"

class Menu : public View {
   public:
	Menu(ImGuiIO& io);
	void draw();
	bool keepActive;
	ImGuiIO* IO;

   private:
	std::vector<View*> views;
	std::vector<View*> activeSimulations;
};

#endif