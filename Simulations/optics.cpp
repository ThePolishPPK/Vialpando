#include "optics.hpp"

#include <imgui.h>

#include <algorithm>
#include <tuple>
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
	static vector<Ray> rays;
	static uint8_t raysCount = 4;
	static float raysGap = 0.05f;
	static bool _onlyOnceExecutedScript = []() {
		Lens l1;
		l1.position = {0.3f, 0.325f};
		l1.type = Lens::Type::biconvex;

		lens.push_back(l1);
		return true;
	}();

	ImGui::Begin(tr(this->name).c_str(), &this->keepActive,
				 ImGuiWindowFlags_MenuBar);

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

	// Add rays
	rays.clear();
	for (uint8_t i = 0; i < raysCount; i++) {
		Ray r;
		r.points.push_back({0.05f, 0.25f + raysGap * i});
		rays.push_back(r);
	}

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

	// Calculate Rays
	for (auto& r : rays) {
		tuple<int8_t, ImVec2, float, float> closest = {-1, {0, 0}, 0, 0};
		for (int8_t current = 0; current < lens.size(); current++) {
			Lens& l = lens[current];
			ImVec2& p = r.points.back();
			r._angle = fmodf(r._angle + 2 * M_PI, 2 * M_PI);
			float lensAngles[2] = {angleBetweenPoints(p, l.begin),
								   angleBetweenPoints(p, l.end)};
			float max = std::max(lensAngles[0], lensAngles[1]),
				  min = std::min(lensAngles[0], lensAngles[1]);

			// Checks ray hit lens and update informations
			if ((max - min < M_PI && r._angle < max && r._angle > min) ||
				(max - min > M_PI && (r._angle < min || r._angle > max))) {
				ImVec2 hitPoint = l.position;

				float d = -sin(angleBetweenPoints(p, l.position) - r._angle) *
						  distanceBetweenPoints(p, l.position) /
						  sin(r._angle - l.angle);
				hitPoint.x -= cos(l.angle) * d;
				hitPoint.y -= sin(l.angle) * d;
				float distance = distanceBetweenPoints(p, hitPoint);

				if (distanceBetweenPoints(p, hitPoint) > 1e-6f &&
					(get<0>(closest) == -1 ||
					 distanceBetweenPoints(p, hitPoint) < get<2>(closest))) {
					closest = {current, hitPoint,
							   distanceBetweenPoints(p, hitPoint), d};
				}
			}

			if (current == lens.size() - 1 && get<0>(closest) != -1) {
				Lens& cl = lens[get<0>(closest)];  // Closest lens
				r.points.push_back(get<1>(closest));
				// Angle calculation process from:
				// https://physics.stackexchange.com/questions/690925/what-is-the-angle-of-a-ray-passing-through-a-thin-lens
				if (cl.type == Lens::Type::biconvex)
					r._angle = atan(tan((-M_PI / 2) - cl.angle + r._angle) +
									(get<3>(closest) / cl.principalFocus)) +
							   r._angle;
				if (cl.type == Lens::Type::biconcave) {
					r._angle = -atan(tan((-M_PI / 2) - cl.angle + r._angle) +
									 (get<3>(closest) / cl.principalFocus)) +
							   r._angle;
				}
				closest = {-1, {0, 0}, 0, 0};
				current = -1;
			}
		}
		ImVec2 re = r.points.back();
		re.x += cos(r._angle) * 10;
		re.y += sin(r._angle) * 10;
		r.points.push_back(re);
		r._angle = 0;
	}

	// Draw rays
	for (auto& r : rays) {
		for (auto& p : r.points) {
			draw->PathLineTo(
				ImVec2(p.x * scale + windowPos.x, p.y * scale + windowPos.y));
		}
		draw->PathStroke(ImColor(255, 255, 0), 0, 3);
		ImVec2 copy = r.points.front();
		r.points = {copy};
	}

	// Menu
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(tr("Options").c_str())) {
			ImGui::SliderInt(tr("Rays count").c_str(), (int*)&raysCount, 0, 50);
			ImGui::DragFloat(tr("Rays gap").c_str(), &raysGap, 6e-2f, 1e-3f,
							 100.0f, "%.3f m", ImGuiSliderFlags_Logarithmic);
			ImGui::DragFloat(tr("Scale").c_str(), &scale, 100.0f, 10.0f,
							 1000000.0f, "%.2f px = 1 m",
							 ImGuiSliderFlags_Logarithmic);

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// TODO: Use DRY roule for next three code sections

	// Modify lens
	static Lens* onModify = NULL;
	static int modifySelectedType = -1;
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		if (onModify == NULL) {
			for (auto& l : lens) {
				float distance = distanceBetweenPoints(l.position, mousePos);
				if (distance * scale < 30) {
					onModify = &l;
					modifySelectedType = -1;
					ImGui::OpenPopup("ModifyObject");
				}
			}
		}
		if (onModify == NULL) {
			lens.push_back(Lens{});
			onModify = &(lens.back());
			onModify->position = mousePos;
			modifySelectedType = -1;
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
		ImGui::ListBox("##Types", &modifySelectedType, types,
					   IM_ARRAYSIZE(types));
		if (modifySelectedType != -1) {
			if (modifySelectedType == 0) onModify->type = Lens::Type::biconvex;
			if (modifySelectedType == 1) onModify->type = Lens::Type::biconcave;
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
				l.angle = fmodf(l.angle + 2 * M_PI, M_PI);
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