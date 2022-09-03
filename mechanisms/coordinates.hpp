#pragma once

#define _USE_MATH_DEFINES

#include <math.h>

#include <mechanisms/angle.hpp>
#include <type_traits>

struct Coordinates2D {
	double x;
	double y;
};

struct Coordinates3D {
	double x;
	double y;
	double z;
};

template <typename LocationSystem>
class CoordinatesSystem {
   public:
	virtual LocationSystem getCartesianCoordinates() const = 0;
	virtual double getDistanceToPoint(const LocationSystem &point) const = 0;
};

// Curiously Recurring Template Pattern (CRTP)
template <class CoordinatesSubSystem>
class CoordinatesSystem2D : public virtual CoordinatesSystem<Coordinates2D> {
   public:
	virtual void setLocationFrom2DCoordinates(const Coordinates2D &coordinates) = 0;
};

template <class CoordinatesSubSystem>
class CoordinatesSystem3D : public virtual CoordinatesSystem<Coordinates3D> {
   public:
	virtual void setLocationFrom3DCoordinates(const Coordinates3D &coordinates) = 0;
};

class CoordinatesSystemCartesian2D : public CoordinatesSystem2D<CoordinatesSystemCartesian2D> {
   private:
	Coordinates2D locationPoint = {0.0, 0.0};

   public:
	CoordinatesSystemCartesian2D();
	static CoordinatesSystemCartesian2D fromXY(const double &x, const double &y);
	static CoordinatesSystemCartesian2D from2DCoordinates(const Coordinates2D &coordinates);

	double getDistanceToPoint(const Coordinates2D &point) const;
	Coordinates2D getCartesianCoordinates() const;

	void setLocationFrom2DCoordinates(const Coordinates2D &coordinates);

	double getX();
	double getY();
	void setX(const double &x);
	void setY(const double &y);
};

class CoordinatesSystemCartesian3D : public CoordinatesSystem3D<CoordinatesSystemCartesian3D> {
   private:
	Coordinates3D locationPoint = {0.0, 0.0, 0.0};

   public:
	CoordinatesSystemCartesian3D();
	static CoordinatesSystemCartesian3D fromXYZ(const double &x, const double &y, const double &z);
	static CoordinatesSystemCartesian3D from3DCoordinates(const Coordinates3D &coordinates);

	double getDistanceToPoint(const Coordinates3D &point) const;
	Coordinates3D getCartesianCoordinates() const;

	void setLocationFrom3DCoordinates(const Coordinates3D &coordinates);

	double getX();
	double getY();
	double getZ();
	void setX(const double &x);
	void setY(const double &y);
	void setZ(const double &z);
};

class CoordinatesSystemPolar : public CoordinatesSystem2D<CoordinatesSystemPolar> {
   private:
	double radius = 0.0;
	AngleInRadians angleInRadians = 0.0;

   public:
	CoordinatesSystemPolar();
	static CoordinatesSystemPolar fromAngleAndRadius(const AngleInRadians &angle, const double &radius);
	static CoordinatesSystemPolar from2DCoordinates(const Coordinates2D &coordinates2D);

	double getDistanceToPoint(const Coordinates2D &point) const;
	Coordinates2D getCartesianCoordinates() const;
	static Coordinates2D convertPolarToCartesian(const CoordinatesSystemPolar &polarCoordinate);
	static CoordinatesSystemPolar convertCartesianToPolar(const Coordinates2D &cartesianCoordinate);

	void setLocationFrom2DCoordinates(const Coordinates2D &coordinates);

	AngleInRadians getAngleInRadians();
	void setAngleInRadians(const AngleInRadians &angle);

	double getRadius();
	void setRadius(const double &radius);
};

class CoordinatesSystemSpherical : public CoordinatesSystem3D<CoordinatesSystemSpherical> {
   private:
	double radius = 0.0;
	AngleInRadians polarAngleInRadians = 0.0;
	AngleInRadians azimuthalAngleInRadians = 0.0;

   public:
	CoordinatesSystemSpherical();
	static CoordinatesSystemSpherical fromRadiusPolarAndAzimuthalAngle(const double &radius,
																	   const AngleInRadians &polarAngleInRadians,
																	   const AngleInRadians &azimuthalAngleInRadians);
	static CoordinatesSystemSpherical fromRadiusPolarAndAzimuthalAngle(const double &radius,
																	   const double &polarAngleInRadians,
																	   const double &azimuthalAngleInRadians);
	static CoordinatesSystemSpherical from3DCoordinates(const Coordinates3D &coordinates);

	double getDistanceToPoint(const Coordinates3D &point) const;
	Coordinates3D getCartesianCoordinates() const;

	void setLocationFrom3DCoordinates(const Coordinates3D &coordinates);

	double getRadius();
	void setRadius(const double &radius);

	AngleInRadians getPolarAngleInRadians();
	void setPolarAngleInRadians(const AngleInRadians &angleInRadians);

	AngleInRadians getAzimuthalAngleInRadians();
	void setAzimuthalAngleInRadians(const AngleInRadians &angleInRadians);
};