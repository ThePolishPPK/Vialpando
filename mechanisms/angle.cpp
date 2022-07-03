#include <mechanisms/angle.hpp>
#include <stdexcept>

template class Angle<AngleInTurns>;
template class Angle<AngleInQuadrants>;
template class Angle<AngleInSextants>;
template class Angle<AngleInRadians>;
template class Angle<AngleInHexacontades>;
template class Angle<AngleInBinaryDegrees>;
template class Angle<AngleInDegrees>;
template class Angle<AngleInGrads>;
template class Angle<AngleInMinutes>;
template class Angle<AngleInSeconds>;

constexpr double degreesInCircle = 360.0;

template <class AngleType>
Angle<AngleType>::Angle(const double &angle, const double &stepsPerTurn) : stepsPerTurn(stepsPerTurn), angleInDegrees(0.0) {
	this->angleInDegrees += this->convertRawAngleToRawDegrees(angle);
	if (stepsPerTurn <= 0.0) {
		throw std::invalid_argument("Invalid stepsPerTurn value! Must be higher than 0.");
	}
}

template <class AngleType>
Angle<AngleType>::Angle(const Angle<AngleType> &angle) : stepsPerTurn(angle.stepsPerTurn) {
	this->angleInDegrees = angle.getRawAngleInDegrees();
}

template <class AngleType>
void Angle<AngleType>::normalize() {
	while (this->angleInDegrees >= degreesInCircle) {
		this->angleInDegrees -= degreesInCircle;
	}
	while (this->angleInDegrees < 0) {
		this->angleInDegrees += degreesInCircle;
	}
}

template <class AngleType>
double Angle<AngleType>::getRawAngle() const {
	const double stepPerOneDegree = this->stepsPerTurn / degreesInCircle;
	return this->angleInDegrees * stepPerOneDegree;
}

template <class AngleType>
double Angle<AngleType>::getNormalizedRawAngle() {
	this->normalize();
	return this->getRawAngle();
}

template <class AngleType>
AngleInDegrees Angle<AngleType>::getAngleInDegrees() const {
	return AngleInDegrees(this->angleInDegrees);
}

template <class AngleType>
double Angle<AngleType>::getRawAngleInDegrees() const {
	AngleInDegrees angleInDegrees = this->getAngleInDegrees();
	return angleInDegrees.getRawAngle();
}

template <class AngleType>
AngleType Angle<AngleType>::operator*(const double &multiplier) const {
	double angleInDegrees = this->angleInDegrees * multiplier;
	return AngleType(this->convertRawDegreesToRawAngle(angleInDegrees));
}

template <class AngleType>
AngleType Angle<AngleType>::operator/(const double &divisor) const {
	double angleInDegrees = this->angleInDegrees / divisor;
	return AngleType(this->convertRawDegreesToRawAngle(angleInDegrees));
}

template <class AngleType>
void Angle<AngleType>::operator*=(const double &multiplier) {
	this->angleInDegrees *= multiplier;
}

template <class AngleType>
void Angle<AngleType>::operator/=(const double &divisor) {
	this->angleInDegrees /= divisor;
}

template <class AngleType>
double Angle<AngleType>::convertRawDegreesToRawAngle(const double &degrees) const {
	return degrees * stepsPerTurn / degreesInCircle;
}

template <class AngleType>
double Angle<AngleType>::convertRawAngleToRawDegrees(const double &angle) const {
	return angle * degreesInCircle / stepsPerTurn;
}