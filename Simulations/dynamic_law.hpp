#ifndef DYNAMIC_LAWS_H
#define DYNAMIC_LAWS_H

#include <cmath>

#include "../view.hpp"

class Dynamics : public View {
   public:
	Dynamics();
	void draw();

   private:
	bool skateActive = false, inclinedPlaneActive = false, cannonActive = false;
	void drawSkateboard();
	void drawInclinedPlane();
	void drawCannon();
	struct {
		float inclination = 30;	 // Degree <0, 50>
		float position = 1.0f;	 // Position in range <0, 1>. 1 = Begin, 0 = End
		float mass = 5.0f;		 // Mass of object in kg
		float frictionRatio =
			0.2f;  // Friction between floor and object. frictionRatio >= 0
		float gravity = 9.8;  // Force of gravity in meters per second square
		float gravityForce;	  // Gravity force working object in Netwons
		float frictionForce;  // Friction force working on object in Newtons
		float slipForce;	  // Force working on object to slip in Netwons
		float pressureForce;  // Force working prependicuarly to flat
	} i;
	float heightOnSlope(
		float angle,
		float pos = 1.0f);	// Return ratio between slope heaight and length
};

#endif