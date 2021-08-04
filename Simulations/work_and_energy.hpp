#ifndef WORK_AND_ENERGY_H
#define WORK_AND_ENERGY_H

#include <imgui.h>

#include "../view.hpp"

class WorkAndEnergy : public View {
   public:
	WorkAndEnergy();
	void draw();

   private:
	double lastUpdate = 0;		  // Last update time
	float baseLength = 0.1;		  // Length of spring
	float coilLength = 0.02;	  // Length of spring part
	int coilsCount = 30;		  // Count of spring parts
	float k = 0.2;				  // Elastic constant
	float speed = 0;			  // Speed of spring. Relatively to x axis.
	float position = baseLength;  // Position of spring end related to begin
	float scale = 3000.0f;		  // scale pixels = 1 meter
	struct {
		float x = 0;		  // Position x
		float y = 0;		  // Position y
		float width = 0.04;	  // Width of box
		float height = 0.03;  // Height of box
		float mass = 0.1;	  // Mass of box
	} pkg;
};

#endif