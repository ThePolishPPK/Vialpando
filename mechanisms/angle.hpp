#pragma once

#include <cmath>
#include <type_traits>

class AngleInDegrees;

// TODO: Angle<>::fromDegrees (not implemented yet) should be static method,
// but stepsPerTurn value aren't initialized as constant.

// Curiously Recurring Template Pattern (CRTP)
template <class AngleType>
class Angle {
   private:
	double angleInDegrees = 0.0;
	const double stepsPerTurn;

	double convertRawDegreesToRawAngle(const double &degrees) const;
	double convertRawAngleToRawDegrees(const double &angle) const;

   public:
	Angle(const double &angle, const double &stepsPerTurn);
	Angle(const double &angle);
	Angle(const Angle &angle);
	static AngleType fromDegrees(const double &angleInDegrees);

	void normalize();

	double getRawAngle() const;
	double getNormalizedRawAngle();
	AngleInDegrees getAngleInDegrees() const;
	double getRawAngleInDegrees() const;

	AngleType operator*(const double &multiplier) const;
	AngleType operator/(const double &divisor) const;
	void operator*=(const double &multiplier);
	void operator/=(const double &divisor);

	void operator=(const Angle<AngleType> &angle) {
		this->angleInDegrees = angle.getRawAngleInDegrees();
	}

	template <class AngleSecondType, typename = std::enable_if<std::is_base_of<Angle, AngleSecondType>::value, bool>>
	AngleType operator+(const AngleSecondType &angle) const {
		double angleInDegrees = this->angleInDegrees + angle.getRawAngleInDegrees();
		return AngleType(this->convertRawDegreesToRawAngle(angleInDegrees));
	}

	template <class AngleSecondType, typename = std::enable_if<std::is_base_of<Angle, AngleSecondType>::value, bool>>
	AngleType operator-(const AngleSecondType &angle) const {
		double angleInDegrees = this->angleInDegrees - angle.getRawAngleInDegrees();
		return AngleType(this->convertRawDegreesToRawAngle(angleInDegrees));
	}

	template <class AngleSecondType, typename = std::enable_if<std::is_base_of<Angle, AngleSecondType>::value, bool>>
	void operator+=(const AngleSecondType &angle) {
		this->angleInDegrees += angle.getRawAngleInDegrees();
	}

	template <class AngleSecondType, typename = std::enable_if<std::is_base_of<Angle, AngleSecondType>::value, bool>>
	void operator-=(const AngleSecondType &angle) {
		this->angleInDegrees -= angle.getRawAngleInDegrees();
	}
};

class AngleInTurns : public Angle<AngleInTurns> {
   public:
	AngleInTurns(const double &angle) : Angle(angle, 1.0){};
};

class AngleInQuadrants : public Angle<AngleInQuadrants> {
   public:
	AngleInQuadrants(const double &angle) : Angle(angle, 4.0){};
};

class AngleInSextants : public Angle<AngleInSextants> {
   public:
	AngleInSextants(const double &angle) : Angle(angle, 6.0){};
};

class AngleInRadians : public Angle<AngleInRadians> {
   public:
	AngleInRadians(const double &angle) : Angle(angle, 2.0 * M_PI){};
};

class AngleInHexacontades : public Angle<AngleInHexacontades> {
   public:
	AngleInHexacontades(const double &angle) : Angle(angle, 60.0){};
};

class AngleInBinaryDegrees : public Angle<AngleInBinaryDegrees> {
   public:
	AngleInBinaryDegrees(const double &angle) : Angle(angle, 256.0){};
};

class AngleInDegrees : public Angle<AngleInDegrees> {
   public:
	AngleInDegrees(const double &angle) : Angle(angle, 360.0){};
};

class AngleInGrads : public Angle<AngleInGrads> {
   public:
	AngleInGrads(const double &angle) : Angle(angle, 400.0){};
};

class AngleInMinutes : public Angle<AngleInMinutes> {
   public:
	AngleInMinutes(const double &angle) : Angle(angle, 360.0 * 60.0){};
};

class AngleInSeconds : public Angle<AngleInSeconds> {
   public:
	AngleInSeconds(const double &angle) : Angle(angle, 360.0 * 60.0 * 60.0){};
};