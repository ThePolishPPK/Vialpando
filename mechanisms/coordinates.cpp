#include <cmath>
#include <mechanisms/angle.hpp>
#include <mechanisms/coordinates.hpp>
#include <stdexcept>

/**
 * Coordinates System Cartesian 2D
 */

CoordinatesSystemCartesian2D::CoordinatesSystemCartesian2D() {
}

CoordinatesSystemCartesian2D CoordinatesSystemCartesian2D::fromXY(const double &x, const double &y) {
	Coordinates2D coordinates2D({x, y});
	return CoordinatesSystemCartesian2D::from2DCoordinates(coordinates2D);
}

CoordinatesSystemCartesian2D CoordinatesSystemCartesian2D::from2DCoordinates(const Coordinates2D &coordinates) {
	CoordinatesSystemCartesian2D coordinatesSystem;
	coordinatesSystem.setLocationFrom2DCoordinates(coordinates);
	return coordinatesSystem;
}

double CoordinatesSystemCartesian2D::getDistanceToPoint(const Coordinates2D &point) const {
	double diffX = this->locationPoint.x - point.x;
	double diffY = this->locationPoint.y - point.y;
	double squareDistance = pow(diffX, 2) + pow(diffY, 2);
	double distance = sqrt(squareDistance);
	return distance;
}

Coordinates2D CoordinatesSystemCartesian2D::getCartesianCoordinates() const {
	return Coordinates2D(this->locationPoint);
}

void CoordinatesSystemCartesian2D::setLocationFrom2DCoordinates(const Coordinates2D &coordinates) {
	this->locationPoint = coordinates;
}

double CoordinatesSystemCartesian2D::getX() {
	return this->locationPoint.x;
}

double CoordinatesSystemCartesian2D::getY() {
	return this->locationPoint.y;
}

void CoordinatesSystemCartesian2D::setX(const double &x) {
	this->locationPoint.x = x;
}

void CoordinatesSystemCartesian2D::setY(const double &y) {
	this->locationPoint.y = y;
}

/**
 * Coordinates System Cartesian 3D
 */

CoordinatesSystemCartesian3D::CoordinatesSystemCartesian3D() {
}

CoordinatesSystemCartesian3D CoordinatesSystemCartesian3D::fromXYZ(const double &x, const double &y, const double &z) {
	return CoordinatesSystemCartesian3D::from3DCoordinates({x, y, z});
}

CoordinatesSystemCartesian3D CoordinatesSystemCartesian3D::from3DCoordinates(const Coordinates3D &coordinates) {
	CoordinatesSystemCartesian3D coordinatesSystem;
	coordinatesSystem.setLocationFrom3DCoordinates(coordinates);
	return coordinatesSystem;
}

double CoordinatesSystemCartesian3D::getDistanceToPoint(const Coordinates3D &point) const {
	double cubeDiffX = pow(point.x - this->locationPoint.x, 2);
	double cubeDiffY = pow(point.y - this->locationPoint.y, 2);
	double cubeDiffZ = pow(point.z - this->locationPoint.z, 2);
	double distance = sqrt(cubeDiffX + cubeDiffY + cubeDiffZ);
	return distance;
}

Coordinates3D CoordinatesSystemCartesian3D::getCartesianCoordinates() const {
	return this->locationPoint;
}

void CoordinatesSystemCartesian3D::setLocationFrom3DCoordinates(const Coordinates3D &coordinates) {
	this->locationPoint = coordinates;
}

double CoordinatesSystemCartesian3D::getX() {
	return this->locationPoint.x;
}

double CoordinatesSystemCartesian3D::getY() {
	return this->locationPoint.y;
}

double CoordinatesSystemCartesian3D::getZ() {
	return this->locationPoint.z;
}

void CoordinatesSystemCartesian3D::setX(const double &x) {
	this->locationPoint.x = x;
}

void CoordinatesSystemCartesian3D::setY(const double &y) {
	this->locationPoint.y = y;
}

void CoordinatesSystemCartesian3D::setZ(const double &z) {
	this->locationPoint.z = z;
}

/**
 * Coordinates System Polar
 */

CoordinatesSystemPolar::CoordinatesSystemPolar() {
}

CoordinatesSystemPolar CoordinatesSystemPolar::fromAngleAndRadius(const AngleInRadians &angle, const double &radius) {
	CoordinatesSystemPolar coordinatesSystem;
	coordinatesSystem.setAngleInRadians(angle);
	coordinatesSystem.setRadius(radius);
	return coordinatesSystem;
}

CoordinatesSystemPolar CoordinatesSystemPolar::from2DCoordinates(const Coordinates2D &coordinates2D) {
	CoordinatesSystemPolar coordinatesSystem;
	coordinatesSystem.setLocationFrom2DCoordinates(coordinates2D);
	return coordinatesSystem;
}

double CoordinatesSystemPolar::getDistanceToPoint(const Coordinates2D &point) const {
	Coordinates2D currentLocation = this->getCartesianCoordinates();
	CoordinatesSystemCartesian2D currentLocationSystem = CoordinatesSystemCartesian2D::from2DCoordinates(currentLocation);
	return currentLocationSystem.getDistanceToPoint(point);
}

Coordinates2D CoordinatesSystemPolar::getCartesianCoordinates() const {
	return CoordinatesSystemPolar::convertPolarToCartesian(*this);
}

Coordinates2D CoordinatesSystemPolar::convertPolarToCartesian(const CoordinatesSystemPolar &polarCoordinate) {
	double x = polarCoordinate.radius * cos(polarCoordinate.angleInRadians.getRawAngle());
	double y = polarCoordinate.radius * sin(polarCoordinate.angleInRadians.getRawAngle());
	return Coordinates2D({x, y});
}

CoordinatesSystemPolar CoordinatesSystemPolar::convertCartesianToPolar(const Coordinates2D &cartesianCoordinate) {
	double radius = CoordinatesSystemCartesian2D::fromXY(0, 0).getDistanceToPoint(cartesianCoordinate);
	double rawAngleInRadians = 0.0;
	if (radius > 0.0) {
		if (cartesianCoordinate.y >= 0) {
			rawAngleInRadians = acos(cartesianCoordinate.x / radius);
		} else {
			rawAngleInRadians = -acos(cartesianCoordinate.x / radius);
		}
	}
	return CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(rawAngleInRadians), radius);
}

void CoordinatesSystemPolar::setLocationFrom2DCoordinates(const Coordinates2D &coordinates) {
	CoordinatesSystemPolar polarSystem = CoordinatesSystemPolar::convertCartesianToPolar(coordinates);
	this->angleInRadians = polarSystem.angleInRadians;
	this->radius = polarSystem.radius;
}

AngleInRadians CoordinatesSystemPolar::getAngleInRadians() {
	return this->angleInRadians;
}

void CoordinatesSystemPolar::setAngleInRadians(const AngleInRadians &angle) {
	this->angleInRadians = angle;
}

double CoordinatesSystemPolar::getRadius() {
	return this->radius;
}

void CoordinatesSystemPolar::setRadius(const double &radius) {
	if (radius < 0.0) {
		throw std::invalid_argument("Radius of Polar System Coordinates cannot be lower than zero!");
	}
	this->radius = radius;
}

/**
 * Coordinates System Spherical
 */

CoordinatesSystemSpherical::CoordinatesSystemSpherical() {
}

CoordinatesSystemSpherical CoordinatesSystemSpherical::fromRadiusPolarAndAzimuthalAngle(const double &radius,
																						const AngleInRadians &polarAngleInRadians,
																						const AngleInRadians &azimuthalAngleInRadians) {
	CoordinatesSystemSpherical coordinatesSystem;
	coordinatesSystem.setRadius(radius);
	coordinatesSystem.polarAngleInRadians = polarAngleInRadians;
	coordinatesSystem.azimuthalAngleInRadians = azimuthalAngleInRadians;

	return coordinatesSystem;
}

CoordinatesSystemSpherical CoordinatesSystemSpherical::fromRadiusPolarAndAzimuthalAngle(const double &radius,
																						const double &polarAngleInRadians,
																						const double &azimuthalAngleInRadians) {
	return fromRadiusPolarAndAzimuthalAngle(radius, AngleInRadians(polarAngleInRadians), AngleInRadians(azimuthalAngleInRadians));
}

double CoordinatesSystemSpherical::getDistanceToPoint(const Coordinates3D &point) const {
	Coordinates3D coordinates = this->getCartesianCoordinates();
	double sumOfCoordinatesDiffPow = pow(coordinates.x - point.x, 2) + pow(coordinates.y - point.y, 2) + pow(coordinates.z - point.z, 2);
	return sqrt(sumOfCoordinatesDiffPow);
}

Coordinates3D CoordinatesSystemSpherical::getCartesianCoordinates() const {
	const double &polarAngle = this->polarAngleInRadians.getRawAngle();
	const double &azimuthalAngle = this->azimuthalAngleInRadians.getRawAngle();

	const double polarAngleSine = std::sin(polarAngle);
	const double polarAngleCosine = std::cos(polarAngle);

	const double azimuthalAngleSine = std::sin(azimuthalAngle);
	const double azimuthalAngleCosine = std::cos(azimuthalAngle);

	double xCoordinate = this->radius * polarAngleSine * azimuthalAngleCosine;
	double yCoordinate = this->radius * polarAngleSine * azimuthalAngleSine;
	double zCoordinate = this->radius * polarAngleCosine;

	return {xCoordinate, yCoordinate, zCoordinate};
}

CoordinatesSystemSpherical CoordinatesSystemSpherical::from3DCoordinates(const Coordinates3D &coordinates) {
	CoordinatesSystemSpherical system;
	system.setLocationFrom3DCoordinates(coordinates);

	return system;
}

void CoordinatesSystemSpherical::setLocationFrom3DCoordinates(const Coordinates3D &coordinates) {
	double radius, polarRawRadiusAngle, azimutalRawRadiusAngle;

	double radiusSquare = pow(coordinates.x, 2) + pow(coordinates.y, 2) + pow(coordinates.z, 2);
	radius = sqrt(radiusSquare);

	polarRawRadiusAngle = acos(coordinates.z / radius);

	azimutalRawRadiusAngle = atan(coordinates.y / coordinates.x);

	this->radius = radius;
	this->polarAngleInRadians = polarRawRadiusAngle;
	this->azimuthalAngleInRadians = azimutalRawRadiusAngle;
}

double CoordinatesSystemSpherical::getRadius() {
	return this->radius;
}

void CoordinatesSystemSpherical::setRadius(const double &radius) {
	if (radius >= 0) {
		this->radius = radius;
	} else {
		throw std::invalid_argument("Radius cannot be negative!");
	}
}

AngleInRadians CoordinatesSystemSpherical::getPolarAngleInRadians() {
	return this->polarAngleInRadians;
}

void CoordinatesSystemSpherical::setPolarAngleInRadians(const AngleInRadians &angleInRadians) {
	this->polarAngleInRadians = angleInRadians;
}

AngleInRadians CoordinatesSystemSpherical::getAzimuthalAngleInRadians() {
	return this->azimuthalAngleInRadians;
}

void CoordinatesSystemSpherical::setAzimuthalAngleInRadians(const AngleInRadians &angleInRadians) {
	this->azimuthalAngleInRadians = angleInRadians;
}