#include "dynamic_law.hpp"

#include <imgui.h>

#include <algorithm>

#include "../basic.hpp"
#include "../translate.hpp"

Dynamics::Dynamics() {
	this->name = "Dynamic Laws";
	this->keepActive = true;
	this->io = &ImGui::GetIO();
}

void Dynamics::draw() {
	if (ImGui::Begin(tr(this->name).c_str(), &this->keepActive,
					 ImGuiWindowFlags_NoResize)) {
		ImGui::Checkbox(tr("Inclined plane").c_str(),
						&this->inclinedPlaneActive);
		ImGui::Checkbox(tr("Cannon").c_str(), &this->cannonActive);
	}
	ImGui::End();

	if (this->inclinedPlaneActive) {
		ImGui::Begin(
			(tr("Dynamic Laws") + " - " + tr("Inclined plane")).c_str(),
			&this->inclinedPlaneActive,
			ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(tr("Options").c_str())) {
				ImGui::SetNextItemWidth(128);
				ImGui::DragFloat(tr("Force vector scale").c_str(),
								 &this->i.forceScale, 0.5f, 0.001f,
								 std::pow(2, 12), "%.2f : 1",
								 ImGuiSliderFlags_AlwaysClamp |
									 ImGuiSliderFlags_Logarithmic);
				ImGui::SetNextItemWidth(128);
				ImGui::DragFloat(tr("Time speed").c_str(), &this->i.timeScale,
								 0.5f, 0.001f, std::pow(2, 12), "%.2f : 1",
								 ImGuiSliderFlags_AlwaysClamp |
									 ImGuiSliderFlags_Logarithmic);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		drawInclinedPlane();
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(
			tr("Mass").c_str(), &this->i.mass, 5.0f, 0.01f, std::pow(2, 16),
			"%.2f kg",
			ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Gravity").c_str(), &this->i.gravity, 0.1f, 0.1f,
						 std::pow(2, 8), "%.1f m/s²",
						 ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Angle").c_str(), &this->i.inclination, 0.1f, 0.0f,
						 90.0f, "%.1f°", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(tr("Friction ratio").c_str(), &this->i.frictionRatio,
						 0.004f, 0.0f, 100.0f, "%.2f",
						 ImGuiSliderFlags_AlwaysClamp);
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat(
			tr("Distance").c_str(), &this->i.distance, 0.5f, 0.001f,
			std::pow(2, 12), "%.2f m",
			ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_Logarithmic);
		if (ImGui::Button(tr("Move up").c_str())) {
			this->i.position = 1.0f;
		}

		this->i.gravityForce = this->i.gravity * this->i.mass;
		this->i.slipForce =
			std::sin((this->i.inclination / 180) * M_PI) * this->i.gravityForce;
		this->i.pressureForce =
			std::cos((this->i.inclination / 180) * M_PI) * this->i.gravityForce;
		this->i.frictionForce = this->i.frictionRatio * this->i.pressureForce;
		if (this->i.frictionForce > this->i.slipForce && this->i.speed <= 0)
			this->i.frictionForce = this->i.slipForce;

		float timeDelta = ImGui::GetTime() - this->i.lastRefresh,
			  part = timeDelta / this->i.timeScale;
		if (timeDelta <= 1) {
			if (this->i.position > 0) {
				this->i.position -= this->i.speed * part / this->i.distance;
				this->i.speed += (this->i.slipForce - this->i.frictionForce) *
								 part / this->i.mass;
				if (this->i.speed < 0) this->i.speed = 0;
			} else {
				this->i.speed = 0;
			}
		}
		this->i.lastRefresh = ImGui::GetTime();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - 200, 60));

		ImGui::Text((tr("Speed") + ": %.2fm/s\n" + tr("Gravity force") +
					 ": %.1fN\n" + tr("Friction force") + ": %.1fN\n" +
					 tr("Slip force") + ": %.1fN\n" + tr("Pressure force") +
					 ": %.1fN\n" + tr("Ground reaction") + ": %.1fN\n")
						.c_str(),
					this->i.speed, this->i.gravityForce, this->i.frictionForce,
					this->i.slipForce, this->i.pressureForce,
					this->i.pressureForce);

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::InvisibleButton("#inclinedPlane", ImGui::GetWindowSize());
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) &&
			ImGui::IsItemFocused() && ImGui::IsItemActive()) {
			ImVec2 delta = this->io->MouseDelta;
			this->i.viewX += delta.x;
			this->i.viewY += delta.y;
		}

		ImGui::End();
	}

	if (this->cannonActive) {
		ImGui::Begin((tr("Dynamic Laws") + " - " + tr("Cannon")).c_str(),
					 &this->cannonActive, ImGuiWindowFlags_MenuBar);
		this->drawCannonSimulation();
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(tr("Options").c_str())) {
				ImGui::DragFloat(tr("Cannon mass").c_str(), &this->c.mass, 1, 1,
								 2048, "%.0f kg");
				ImGui::DragFloat(tr("Bullet mass").c_str(),
								 &this->c.currentBulletMass, 0.1, 0.1, 2048,
								 "%.1f kg");
				ImGui::DragFloat(tr("Bullet speed").c_str(),
								 &this->c.currentBulletSpeed, 0.1, 0.1, 2048,
								 "%.1f m/s");
				ImGui::DragFloat(tr("Bullet radius").c_str(),
								 &this->c.bulletSize, 0.01, 0.01, 64, "%.2f m");
				ImGui::DragFloat(tr("Window scale").c_str(), &this->c.boxScale,
								 0.1, 0.1, 1024, "%.1fpx = 1m");
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
}

void Dynamics::drawInclinedPlane() {
	ImDrawList* draw = ImGui::GetWindowDrawList();

	ImVec2 wPos = ImGui::GetWindowPos();
	ImVec2 wSize = ImGui::GetWindowSize();

	if (this->io->MouseWheel != 0) {
		this->i.scale *= 1 - (this->io->MouseWheel / 10);
	}

	ImVec2 p1(wPos.x + this->i.viewX,
			  wPos.y + (wSize.y / this->i.scale) + this->i.viewY),
		p2(p1.x + (wSize.x / this->i.scale), p1.y);
	ImVec2 p3(p1.x,
			  p1.y - this->heightOnSlope(this->i.inclination) * (p2.x - p1.x));

	draw->AddTriangleFilled(p1, p2, p3, ImColor(0, 64, 255));  // Slope

	float slopeAngle = (this->i.inclination / 180) * M_PI, w = p2.x - p1.x,
		  pos = 1 - this->i.position;

	ImVec2 q1(p1.x + (pos * w),
			  p1.y - this->i.position * w * std::tan(slopeAngle)),
		diff(std::cos(slopeAngle) * 50, std::sin(slopeAngle) * 50);
	ImVec2 q2(q1.x + diff.x, q1.y + diff.y), q3(q2.x + diff.y, q2.y - diff.x),
		q4(q1.x + diff.y, q1.y - diff.x);

	draw->AddQuadFilled(q1, q2, q3, q4, ImColor(255, 0, 0));  // Square on slope

	ImVec2 v0((q1.x + q3.x) / 2, (q1.y + q3.y) / 2),
		vg(v0.x, v0.y + (this->i.gravityForce / this->i.forceScale)),
		vf(q1.x - this->i.frictionForce * std::cos(slopeAngle) /
					  this->i.forceScale,
		   q1.y - this->i.frictionForce * std::sin(slopeAngle) /
					  this->i.forceScale),
		vs(v0.x + this->i.slipForce * std::cos(slopeAngle) / this->i.forceScale,
		   v0.y +
			   this->i.slipForce * std::sin(slopeAngle) / this->i.forceScale),
		vp(this->i.pressureForce * std::sin(slopeAngle) / this->i.forceScale,
		   this->i.pressureForce * std::cos(slopeAngle) / this->i.forceScale);

	draw->AddCircleFilled(v0, 3, ImColor(0, 255, 0));  // Center of many forces
	drawArrow(v0, vg, draw);						   // Gravity force vector
	drawArrow(q1, vf, draw);						   // Friction force vector
	drawArrow(v0, vs, draw);						   // Slip force vector
	drawArrow(v0, ImVec2(v0.x - vp.x, v0.y + vp.y),
			  draw);  // Pressure force vector
	drawArrow(v0, ImVec2(v0.x + vp.x, v0.y - vp.y),
			  draw);  // Ground reaction force vector

	draw->PushClipRect(wPos, ImVec2(wPos.x + wSize.x, wPos.y + wSize.y));
}

void Dynamics::drawCannonSimulation() {
	ImVec2 wPos = ImGui::GetWindowPos(), wSize = ImGui::GetWindowSize();
	static ImDrawList* draw = ImGui::GetWindowDrawList();
	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImGui::InvisibleButton("", ImVec2(250, 250));
	ImGui::SetCursorPos(ImVec2(wSize.x - 200, cursorPos.y));
	ImGui::Text(
		(tr("Cannon speed") + ": %.1fm/s\n" + tr("Bullet mass") + ": %.1fkg\n" +
		 tr("Bullet speed") + ": %.1fm/s\n")
			.c_str(),
		std::sqrt(std::pow(this->c.move.x, 2) + std::pow(this->c.move.y, 2)),
		this->c.currentBulletMass, this->c.currentBulletSpeed);

	for (auto& bullet : this->c.bullets) {
		draw->AddCircleFilled(bullet.position,
							  this->c.bulletSize * this->c.boxScale,
							  ImColor(70, 70, 70));
	}

	ImVec2 barrelPos =
		this->drawCannon(ImVec2(this->c.position.x * this->c.boxScale + wPos.x,
								this->c.position.y * this->c.boxScale + wPos.y),
						 this->c.rotate, this->c.barrelAngle,
						 this->c.bulletSize * this->c.boxScale);

	float delta = ImGui::GetTime() - this->c.lastUpdate;
	if (delta <= 1) {
		float padding = 3.5 * this->c.bulletSize * this->c.boxScale;
		ImVec2 posLimitX(padding / this->c.boxScale,
						 (wSize.x - padding) / this->c.boxScale),
			posLimitY(2 * padding / this->c.boxScale,
					  wSize.y / this->c.boxScale);
		this->c.move.y -= this->c.gravity * delta;

		this->c.move.x *= std::pow(this->c.moveOportunityInAir, delta);
		this->c.move.y *= std::pow(this->c.moveOportunityInAir, delta);

		this->c.position.x += this->c.move.x * delta;
		this->c.position.y -= this->c.move.y * delta;

		if (this->c.position.y >= posLimitY.y) {
			this->c.move.x *= std::pow(this->c.moveOportunityOnGround, delta);
		}

		if (this->c.position.x < posLimitX.x ||
			this->c.position.x > posLimitX.y) {
			this->c.move.x = 0;
			this->c.position.x =
				(this->c.position.x < posLimitX.x) ? posLimitX.x : posLimitX.y;
		}
		if (this->c.position.y < posLimitY.x ||
			this->c.position.y > posLimitY.y) {
			this->c.move.y = 0;
			this->c.position.y =
				(this->c.position.y < posLimitY.x) ? posLimitY.x : posLimitY.y;
		}

		for (unsigned short bulletID = 0; bulletID < this->c.bullets.size();
			 bulletID++) {
			object& bullet = this->c.bullets[bulletID];
			if (bullet.position.x < 0 || bullet.position.x > wSize.x ||
				bullet.position.y < 0 || bullet.position.y > wSize.y) {
				this->c.bullets.erase(this->c.bullets.begin() + bulletID);
			}
			bullet.move.y -= this->c.gravity * delta;
			bullet.position.x += bullet.move.x * delta * this->c.boxScale;
			bullet.position.y -= bullet.move.y * delta * this->c.boxScale;
		}

		if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
			this->c.barrelAngle += M_PI / 6 * delta;
		}
		if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
			this->c.barrelAngle -= M_PI / 6 * delta;
		}
		draw->PushClipRectFullScreen();
	}
	this->c.lastUpdate = ImGui::GetTime();

	if (this->io->MouseDown[ImGuiMouseButton_Right] &&
		ImGui::IsWindowFocused()) {
		ImVec2& mPos = this->io->MousePos;
		this->c.barrelAngle = -angleBetweenPoints(
			this->c.position, ImVec2((mPos.x - wPos.x) / this->c.boxScale,
									 (mPos.y - wPos.y) / this->c.boxScale));
	}

	if ((this->io->MouseReleased[ImGuiMouseButton_Left] ||
		 ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Tab)) ||
		 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space))) &&
		ImGui::IsWindowFocused()) {
		object bullet;
		bullet.position = barrelPos;
		bullet.mass = this->c.currentBulletMass;
		bullet.move.x =
			std::cos(this->c.barrelAngle) * this->c.currentBulletSpeed;
		bullet.move.y =
			std::sin(this->c.barrelAngle) * this->c.currentBulletSpeed;

		float part = bullet.mass / this->c.mass;
		this->c.move.x += -bullet.move.x * part;
		this->c.move.y += -bullet.move.y * part;

		this->c.bullets.push_back(bullet);
	}
}

ImVec2 Dynamics::drawCannon(const ImVec2& pos, const float& angle,
							const float& aimAngle, const float& size) {
	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImVec2 wheelDiff(std::sin(angle) * size * 2, std::cos(angle) * size * 2),
		distanceDiff((size * 2.5) * std::cos(angle),
					 (size * 2.5) * std::sin(angle)),
		wheel1(pos.x - distanceDiff.x - wheelDiff.x,
			   pos.y + distanceDiff.y - wheelDiff.y),
		wheel2(pos.x + distanceDiff.x - wheelDiff.x,
			   pos.y - distanceDiff.y - wheelDiff.y);
	ImVec2 cc(pos.x - std::sin(angle) * 4 * size,
			  pos.y - std::cos(angle) * 4 * size);

	draw->AddCircleFilled(cc, size * 3, ImColor(125, 125, 125));
	ImVec2 barrelDiff(std::sin(aimAngle) * size * 1.04,
					  std::cos(aimAngle) * size * 1.04);
	ImVec2 barrel[4] = {ImVec2(cc.x - barrelDiff.x, cc.y - barrelDiff.y),
						ImVec2(cc.x + barrelDiff.x, cc.y + barrelDiff.y),
						ImVec2(cc.x + barrelDiff.x + barrelDiff.y * 6,
							   cc.y + barrelDiff.y - barrelDiff.x * 6),
						ImVec2(cc.x - barrelDiff.x + barrelDiff.y * 6,
							   cc.y - barrelDiff.y - barrelDiff.x * 6)};

	draw->AddConvexPolyFilled(&barrel[0], 4, ImColor(125, 125, 125));

	draw->AddCircleFilled(wheel1, size * 2, ImColor(185, 60, 0));
	draw->AddCircleFilled(wheel2, size * 2, ImColor(185, 60, 0));

	ImVec2 out((barrel[2].x + barrel[3].x) / 2,
			   (barrel[2].y + barrel[3].y) / 2);

	draw->PushClipRectFullScreen();
	return ImVec2((barrel[2].x + barrel[3].x) / 2,
				  (barrel[2].y + barrel[3].y) / 2);
}

float Dynamics::heightOnSlope(float angle, float pos) {
	return std::tan((angle / 180) * M_PI) * pos;
}