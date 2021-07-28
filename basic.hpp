#ifndef SHAPES_H
#define SHAPES_H

#include <imgui.h>

float angleBetweenPoints(const ImVec2& from, const ImVec2& to);
void drawArrow(const ImVec2& start, const ImVec2& end, ImDrawList* drawList,
			   float arrowLength = 10.0f, float arrowAngle = 30.0f,
			   float thickness = 3.0f, ImColor color = ImColor(255, 255, 255));

#endif