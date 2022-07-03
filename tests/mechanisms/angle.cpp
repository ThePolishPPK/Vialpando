#include <gtest/gtest.h>

#include <cmath>
#include <mechanisms/angle.hpp>
#include <stdexcept>

TEST(AngleTest, ConstructorWithStepsParametersAndAngle) {
	EXPECT_NO_THROW(Angle<AngleInDegrees>(3.21, 0.32));
	EXPECT_NO_THROW(Angle<AngleInTurns>(31.3, 0.0000000001));
	EXPECT_NO_THROW(Angle<AngleInRadians>(-76.3, 1000.0221));

	EXPECT_THROW(Angle<AngleInRadians>(23.5, -0.0324), std::invalid_argument);
	EXPECT_THROW(Angle<AngleInRadians>(9230.0, 0.0), std::invalid_argument);
}

TEST(AngleSubClassTest, getAngleInDegrees) {
	EXPECT_DOUBLE_EQ((new AngleInTurns(61.42))->getRawAngleInDegrees(), 61.42 * 360.0);
	EXPECT_DOUBLE_EQ((new AngleInQuadrants(52.1))->getRawAngleInDegrees(), 52.1 * 90.0);
	EXPECT_DOUBLE_EQ((new AngleInSextants(123.44))->getRawAngleInDegrees(), 123.44 * 60.0);
	EXPECT_DOUBLE_EQ((new AngleInRadians(81.939))->getRawAngleInDegrees(), 81.939 / (2.0 * M_PI) * 360.0);
	EXPECT_DOUBLE_EQ((new AngleInHexacontades(94.890))->getRawAngleInDegrees(), 94.890 * 6.0);
	EXPECT_DOUBLE_EQ((new AngleInBinaryDegrees(63.081))->getRawAngleInDegrees(), 63.081 / 256.0 * 360.0);
	EXPECT_DOUBLE_EQ((new AngleInDegrees(563.0))->getRawAngleInDegrees(), 563.0);
	EXPECT_DOUBLE_EQ((new AngleInGrads(4.28))->getRawAngleInDegrees(), 4.28 * 0.9);
	EXPECT_DOUBLE_EQ((new AngleInMinutes(72.91))->getRawAngleInDegrees(), 72.91 / 60.0);
	EXPECT_DOUBLE_EQ((new AngleInSeconds(20.106))->getRawAngleInDegrees(), 20.106 / 3600.0);
}

TEST(AngleSubClassTest, getNormalizedRawAngle) {
	EXPECT_DOUBLE_EQ((new AngleInSextants(22.33))->getAngleInDegrees().getNormalizedRawAngle(), 4.33 * 60.0);
	EXPECT_DOUBLE_EQ((new AngleInRadians(54.89))->getAngleInDegrees().getNormalizedRawAngle(), ((54.89 / (2.0 * M_PI)) - 8.0) * 360.0);
	EXPECT_DOUBLE_EQ((new AngleInGrads(46.458))->getAngleInDegrees().getNormalizedRawAngle(), 41.8122);
	EXPECT_DOUBLE_EQ((new AngleInMinutes(3.428))->getAngleInDegrees().getNormalizedRawAngle(), 3.428 / 60.0);
	EXPECT_DOUBLE_EQ((new AngleInSeconds(87.712))->getAngleInDegrees().getNormalizedRawAngle(), 87.712 / 3600.0);
	EXPECT_DOUBLE_EQ((new AngleInHexacontades(-43.24))->getAngleInDegrees().getNormalizedRawAngle(), 100.56);
	EXPECT_DOUBLE_EQ((new AngleInTurns(-1.37))->getAngleInDegrees().getNormalizedRawAngle(), 226.8);
	EXPECT_DOUBLE_EQ((new AngleInBinaryDegrees(-345.55))->getAngleInDegrees().getNormalizedRawAngle(), 234.0703125);
}

TEST(AngleSubClassTest, operatorAdd) {
	EXPECT_DOUBLE_EQ((AngleInSextants(2.00) + AngleInMinutes(3600.00)).getRawAngleInDegrees(), 180.00);
	EXPECT_DOUBLE_EQ((AngleInQuadrants(2.00) + AngleInBinaryDegrees(80.00)).getRawAngleInDegrees(), 292.50);
	EXPECT_DOUBLE_EQ((AngleInHexacontades(60.8) + AngleInGrads(13.555555555555555555)).getRawAngleInDegrees(), 377.00);
}

TEST(AngleSubClassTest, operatorMinus) {
	EXPECT_DOUBLE_EQ((AngleInDegrees(289.00) - AngleInTurns(2.73)).getRawAngleInDegrees(), -693.8);
	EXPECT_DOUBLE_EQ((AngleInGrads(278.21) - AngleInSeconds(181400.4)).getRawAngleInDegrees(), 200.00);
	EXPECT_DOUBLE_EQ((AngleInBinaryDegrees(231.2) - AngleInHexacontades(33.630833333333333333)).getRawAngleInDegrees(), 123.34);
}

TEST(AngleSubClassTest, operatorAddEqual) {
	AngleInMinutes angle(499.2);
	angle += AngleInGrads(66.311111111111111111);
	EXPECT_DOUBLE_EQ(angle.getRawAngleInDegrees(), 68.00);
}

TEST(AngleSubClassTest, operatorMinusEqual) {
	AngleInRadians angle(1.72 * M_PI);
	angle -= AngleInBinaryDegrees(150.307555555555555555);
	EXPECT_DOUBLE_EQ(angle.getRawAngleInDegrees(), 98.23);
}

TEST(AngleSubClassTest, operatorMultiply) {
	EXPECT_DOUBLE_EQ((AngleInMinutes(732.72) * 6.54).getRawAngleInDegrees(), 79.86648);
}

TEST(AngleSubClassTest, operatorDivide) {
	EXPECT_DOUBLE_EQ((AngleInGrads(2368.4) / 32.433).getRawAngleInDegrees(), 65.721949865877349);
}

TEST(AngleSubClassTest, operatorMultiplyEqual) {
	AngleInQuadrants angle(1.55);
	angle *= 24.542;
	EXPECT_DOUBLE_EQ(angle.getRawAngleInDegrees(), 3423.609);
}

TEST(AngleSubClassTest, operatorDivideEqual) {
	AngleInHexacontades angle(325.44);
	angle /= 4.8;
	EXPECT_DOUBLE_EQ(angle.getRawAngleInDegrees(), 406.8);
}