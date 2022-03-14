#include "optics.hpp"

#include <imgui.h>

#include <algorithm>
#include <vector>

#include "../translate.hpp"

using namespace std;

Optics::Optics() {
	this->keepActive = true;
	this->name = "Optics";
}

void Optics::draw() {
	static float scale = 1000.0f;  // scale px = 1m
	static vector<Lens> lens;
	static bool _onlyOnceExecutedScript = []() {
		Lens l1;
		l1.position = {0.3f, 0.2f};
		l1.type = Lens::Type::biconvex;

		lens.push_back(l1);
		return true;
	}();

	ImGui::Begin(tr(this->name).c_str(), &this->keepActive);

	// Window info
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImGui::GetWindowPos();
	static ImDrawList* draw = ImGui::GetWindowDrawList();
	static ImGuiIO& IO = ImGui::GetIO();

	ImVec2 mousePos = ImGui::GetMousePos();
	mousePos.x -= windowPos.x;
	mousePos.x /= scale;
	mousePos.y -= windowPos.y;
	mousePos.y /= scale;

	// Calculate lens points
	for (auto& l : lens) {
		ImVec2 diff = {cos(l.angle) * l.height / 2,
					   sin(l.angle) * l.height / 2};
		l.begin = {l.position.x + diff.x, l.position.y + diff.y};
		l.end = {l.position.x - diff.x, l.position.y - diff.y};
	}

	// Draw lens
	for (auto& l : lens) {
		ImVec2 begin = {l.begin.x * scale + windowPos.x,
						l.begin.y * scale + windowPos.y},
			   end = {l.end.x * scale + windowPos.x,
					  l.end.y * scale + windowPos.y},
			   center = {l.position.x * scale + windowPos.x,
						 l.position.y * scale + windowPos.y},
			   pointDiff = {
				   l.principalFocus * cos(l.angle - ((float)M_PI / 2)),
				   l.principalFocus * sin(l.angle - ((float)M_PI / 2))};
		// TODO: Draw this better using principal focus
		for (auto& x : {begin, end})
			drawArrow(center, x, draw, 10.0f,
					  (l.type == Lens::Type::biconvex
						   ? 80.0f
						   : (l.type == Lens::Type::biconcave ? 280.0f : 0.0f)),
					  5.0f, ImColor(255, 0, 0));
		for (auto& x : {-1, 1})
			draw->AddCircleFilled(ImVec2{pointDiff.x * scale * x + center.x,
										 pointDiff.y * scale * x + center.y},
								  5.0f, ImColor(255, 0, 0));
	}

	// TODO: Use DRY roule for next three code sections

	// Modify lens
	static Lens* onModify = NULL;
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		if (onModify == NULL) {
			for (auto& l : lens) {
				float distance = distanceBetweenPoints(l.position, mousePos);
				if (distance * scale < 30) {
					onModify = &l;
					ImGui::OpenPopup("ModifyObject");
				}
			}
		}
		if (onModify == NULL) {
			lens.push_back(Lens{});
			onModify = &(lens.back());
			onModify->position = mousePos;
			ImGui::OpenPopup("ModifyObject");
		}
	}
	if (ImGui::BeginPopupModal(
			"ModifyObject", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		ImGui::SetWindowSize({200, 0});
		ImGui::Text("%s", tr("Height").c_str());
		ImGui::DragFloat("##Height", &(onModify->height), 5e-4f, 1e-3f,
						 onModify->principalFocus * 2, "%.3f m",
						 ImGuiSliderFlags_Logarithmic);

		ImGui::Text("%s", tr("Principal focus").c_str());
		ImGui::DragFloat("##PrincipalFocus", &(onModify->principalFocus), 4e-3f,
						 onModify->height / 2, 5.0f, "%.3f m",
						 ImGuiSliderFlags_Logarithmic);

		ImGui::Text("%s", tr("Type").c_str());
		const char* Biconvex = tr("Biconvex").c_str();
		const char* Biconcave = tr("Biconcave").c_str();
		const char* types[] = {Biconvex, Biconcave};
		static int selected = -1;
		ImGui::ListBox("##Types", &selected, types, IM_ARRAYSIZE(types));
		if (selected != -1) {
			if (selected == 0) onModify->type = Lens::Type::biconvex;
			if (selected == 1) onModify->type = Lens::Type::biconcave;
		}

		if (ImGui::Button(tr("Remove").c_str())) {
			auto iter = find(lens.begin(), lens.end(), *onModify);
			lens.erase(iter);
			onModify = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(tr("Close").c_str())) {
			onModify = NULL;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Rotate lens
	if (IO.MouseWheel != 0) {
		for (auto& l : lens) {
			float distance = distanceBetweenPoints(l.position, mousePos);
			if (distance * scale < 30) {
				l.angle += (float)M_PI / 64 * IO.MouseWheel;
			}
		}
	}

	// Move lens
	static Lens* onMove = NULL;
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		if (onMove == NULL) {
			for (auto& l : lens) {
				float distance = distanceBetweenPoints(l.position, mousePos);
				if (distance * scale < 30) {
					onMove = &l;
				}
			}
		}
	} else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && onMove != NULL) {
		onMove->position.x += IO.MouseDelta.x / scale;
		onMove->position.y += IO.MouseDelta.y / scale;
	} else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		onMove = NULL;

	draw->PushClipRectFullScreen();
	ImGui::End();
}