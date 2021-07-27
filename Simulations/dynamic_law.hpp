#ifndef DYNAMIC_LAWS_H
#define DYNAMIC_LAWS_H

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
};

#endif