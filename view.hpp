#ifndef VIEW_H
#define VIEW_H

#include <imgui.h>

class View {
 public:
	virtual void draw() = 0;
	ImGuiIO* IO;
	bool keepActive;
	const char* name;
};

#endif