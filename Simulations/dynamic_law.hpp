#ifndef DYNAMIC_LAWS_H
#define DYNAMIC_LAWS_H

#include <cmath>
#include <vector>

#include "../basic.hpp"
#include "../view.hpp"

class Dynamics : public View {
   public:
	Dynamics();
	void draw();

   private:
	ImGuiIO* io;
	bool inclinedPlaneActive = false, cannonActive = false;
	void drawSkateboard();
	void drawInclinedPlane();
	void drawCannonSimulation();
	struct {
		double lastRefresh;
		float viewX, viewY, scale = 1.0f;
		float squareSize = 50.0f;  // Size of one side
		float speed = 0.0f;		   // Speed of square in m/s
		float timeScale = 1.0f;	   // Time scaled by expr 1/timeScale
		float distance = 5.0f;	   // Distance of plane in m.
		float forceScale = 1.0f;   // Force vectors scaled by expr 1/forceScale
		float inclination = 30;	   // Degree <0, 50>
		float position = 1.0f;	// Position in range <0, 1>. 1 = Begin, 0 = End
		float mass = 5.0f;		// Mass of object in kg
		float frictionRatio =
			0.2f;  // Friction between floor and object. frictionRatio >= 0
		float gravity = 9.8;  // Force of gravity in meters per second square
		float gravityForce;	  // Gravity force working object in Netwons
		float frictionForce;  // Friction force working on object in Newtons
		float slipForce;	  // Force working on object to slip in Netwons
		float pressureForce;  // Force working prependicuarly to flat
	} i;
	struct : object {
		ImVec2 move = ImVec2(0, 0);
		ImVec2 position = ImVec2(200, 200);
		float mass = 40.0f;

		float gravity = 9.8f;			   // Gravity force working on object
		float currentBulletMass = 1.0f;	   // Mass of bullet with can be shoted
		float currentBulletSpeed = 20.0f;  // Speed of bullet with can be shoted
		float bulletSize = 0.30f;		   // Radius of bullet
		float barrelAngle = 0.0f;  // Angle of barrel. Helps to calculate angle
								   // of force after shot
		float boxScale = 50.0f;	   // Scale of window. boxScale pixels = 1 meter
		float moveOportunityInAir = 0.94f;
		float moveOportunityOnGround = 0.8f;
		float lastUpdate;		  // Time of last values update
		float secureMode = true;  // Allows to set negative mass and speed

		std::vector<object> bullets;  // List with all bullets
	} c;
	float heightOnSlope(
		float angle,
		float pos = 1.0f);	// Returns ratio between slope height and length
	ImVec2 drawCannon(const ImVec2& pos, const float& angle,
					  const float& aimAngle, const float& size);
};

#endif