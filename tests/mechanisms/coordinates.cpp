#include <gtest/gtest.h>

#include <mechanisms/coordinates.hpp>
#include <stdexcept>

/**
 * Coordinates System Cartesian 2D
 */

TEST(CoordinatesSystemCartesian2DTest, constructor) {
	ASSERT_NO_THROW(CoordinatesSystemCartesian2D());
}

TEST(CoordinatesSystemCartesian2DTest, fromXY) {
	CoordinatesSystemCartesian2D coordinatesSystem = CoordinatesSystemCartesian2D::fromXY(3.32, -32.75);

	EXPECT_EQ(coordinatesSystem.getX(), 3.32);
	EXPECT_EQ(coordinatesSystem.getY(), -32.75);
}

TEST(CoordinatesSystemCartesian2DTest, from2DCoordinates) {
	CoordinatesSystemCartesian2D coordinatesSystem = CoordinatesSystemCartesian2D::from2DCoordinates({-46.23, -75432.44});

	EXPECT_EQ(coordinatesSystem.getX(), -46.23);
	EXPECT_EQ(coordinatesSystem.getY(), -75432.44);
}

TEST(CoordinatesSystemCartesian2DTest, getDistanceToPoint) {
	Coordinates2D pointOne = {0.0, 0.0}, pointTwo = {53.2, 43.33}, pointThree = {6.4, 5.4}, pointFour = {68.3, -88.0};
	EXPECT_EQ(CoordinatesSystemCartesian2D::from2DCoordinates(pointOne).getDistanceToPoint(pointTwo), 68.6128916458124508);
	EXPECT_EQ(CoordinatesSystemCartesian2D::from2DCoordinates(pointOne).getDistanceToPoint(pointFour), 111.395197383011086);
	EXPECT_EQ(CoordinatesSystemCartesian2D::from2DCoordinates(pointTwo).getDistanceToPoint(pointFour), 132.195230246783110);
	EXPECT_EQ(CoordinatesSystemCartesian2D::from2DCoordinates(pointThree).getDistanceToPoint(pointTwo), 60.2405585963476936);
}

TEST(CoordinatesSystemCartesian2DTest, getCartesianCoordinates) {
	std::vector<Coordinates2D> points = {{4.0, 3.0}, {24.6, 14.9}, {3.6, 83.6}};

	for (const Coordinates2D &point : points) {
		CoordinatesSystemCartesian2D coordinatesSystem = CoordinatesSystemCartesian2D::from2DCoordinates(point);
		Coordinates2D recivedPoint = coordinatesSystem.getCartesianCoordinates();

		EXPECT_DOUBLE_EQ(recivedPoint.x, point.x);
		EXPECT_DOUBLE_EQ(recivedPoint.y, point.y);
	}
}

TEST(CoordinatesSystemCartesian2DTest, setLocationFrom2DCoordinates) {
	CoordinatesSystemCartesian2D coordinatesSystem;
	coordinatesSystem.setLocationFrom2DCoordinates({34.12, -42.4});

	EXPECT_DOUBLE_EQ(coordinatesSystem.getX(), 34.12);
	EXPECT_DOUBLE_EQ(coordinatesSystem.getY(), -42.4);
}

/**
 * Coordinates System Cartesian 3D
 */

TEST(CoordinatesSystemCartesian3DTest, constructor) {
	ASSERT_NO_THROW(CoordinatesSystemCartesian3D());
}

TEST(CoordinatesSystemCartesian3DTest, fromXYZ) {
	CoordinatesSystemCartesian3D coordinatesSystem = CoordinatesSystemCartesian3D::fromXYZ(21.75, 23.93, 56.73);

	EXPECT_EQ(coordinatesSystem.getX(), 21.75);
	EXPECT_EQ(coordinatesSystem.getY(), 23.93);
	EXPECT_EQ(coordinatesSystem.getZ(), 56.73);
}

TEST(CoordinatesSystemCartesian3DTest, from3DCoordinates) {
	CoordinatesSystemCartesian3D coordinatesSystem = CoordinatesSystemCartesian3D::from3DCoordinates({-6.59, 67.22, 2.78});

	EXPECT_EQ(coordinatesSystem.getX(), -6.59);
	EXPECT_EQ(coordinatesSystem.getY(), 67.22);
	EXPECT_EQ(coordinatesSystem.getZ(), 2.78);
}

TEST(CoordinatesSystemCartesian3DTest, getDistanceToPoint) {
	Coordinates3D pointOne = {0.0, 0.0, 0.0}, pointTwo = {53.2, 43.33, 67.48}, pointThree = {6.4, 5.4, 1.98}, pointFour = {68.3, -88.0, 45.55};
	EXPECT_EQ(CoordinatesSystemCartesian3D::from3DCoordinates(pointOne).getDistanceToPoint(pointTwo), 96.2355407321016719);
	EXPECT_EQ(CoordinatesSystemCartesian3D::from3DCoordinates(pointOne).getDistanceToPoint(pointFour), 120.348213530571363);
	EXPECT_EQ(CoordinatesSystemCartesian3D::from3DCoordinates(pointTwo).getDistanceToPoint(pointFour), 134.001879837560488);
	EXPECT_EQ(CoordinatesSystemCartesian3D::from3DCoordinates(pointThree).getDistanceToPoint(pointTwo), 88.9897460385183929);
}

TEST(CoordinatesSystemCartesian3DTest, getCartesianCoordinates) {
	std::vector<Coordinates3D> points = {{20.63, 94.32, 1.05}, {3.35, -78.3, 21.25}, {5.93, 74.55, -52.15}};

	for (const Coordinates3D &point : points) {
		CoordinatesSystemCartesian3D coordinatesSystem = CoordinatesSystemCartesian3D::from3DCoordinates(point);
		Coordinates3D recivedPoint = coordinatesSystem.getCartesianCoordinates();

		EXPECT_DOUBLE_EQ(recivedPoint.x, point.x);
		EXPECT_DOUBLE_EQ(recivedPoint.y, point.y);
		EXPECT_DOUBLE_EQ(recivedPoint.z, point.z);
	}
}

TEST(CoordinatesSystemCartesian3DTest, setLocationFrom3DCoordinates) {
	CoordinatesSystemCartesian3D coordinatesSystem;
	coordinatesSystem.setLocationFrom3DCoordinates({32.5, 542.3, 32.1});

	EXPECT_DOUBLE_EQ(coordinatesSystem.getX(), 32.5);
	EXPECT_DOUBLE_EQ(coordinatesSystem.getY(), 542.3);
	EXPECT_DOUBLE_EQ(coordinatesSystem.getZ(), 32.1);
}

/**
 * Coordinate System Polar
 */

TEST(CoordinatesSystemPolarTest, constructor) {
	ASSERT_NO_THROW(CoordinatesSystemPolar());
}

TEST(CoordinatesSystemPolarTest, fromAngleAndRadius_CorrectData) {
	ASSERT_NO_THROW(CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(2.0), 3.4));
	ASSERT_NO_THROW(CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(-3.14), 0.0));
	ASSERT_THROW(CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(34.22), -32.12), std::invalid_argument);
}

TEST(CoordinatesSystemPolarTest, getDistanceToPoint) {
	CoordinatesSystemPolar pointOne = CoordinatesSystemPolar::from2DCoordinates({0.0, 0.0}), pointTwo = CoordinatesSystemPolar::from2DCoordinates({3.0, 4.0}),
						   pointThree = CoordinatesSystemPolar::from2DCoordinates({7.3, 6.1}),
						   pointFour = CoordinatesSystemPolar::from2DCoordinates({5.22, 4.34});
	EXPECT_DOUBLE_EQ(pointOne.getDistanceToPoint(pointTwo.getCartesianCoordinates()), 5.0);
	EXPECT_DOUBLE_EQ(pointOne.getDistanceToPoint(pointThree.getCartesianCoordinates()), 9.51314879522022403);
	EXPECT_DOUBLE_EQ(pointOne.getDistanceToPoint(pointFour.getCartesianCoordinates()), 6.78851972082279511);
	EXPECT_DOUBLE_EQ(pointTwo.getDistanceToPoint(pointThree.getCartesianCoordinates()), 4.78539444560215955);
	EXPECT_DOUBLE_EQ(pointTwo.getDistanceToPoint(pointFour.getCartesianCoordinates()), 2.24588512618076706);
	EXPECT_DOUBLE_EQ(pointThree.getDistanceToPoint(pointFour.getCartesianCoordinates()), 2.72470181854822419);
}

TEST(CoordinatesSystemPolarTest, getCartesianCoordinates) {
	CoordinatesSystemPolar coordinatesSystemOne = CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(2.3 * M_PI), 3.21),
						   coordinatesSystemTwo = CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(1.63 * M_PI), 32.3);

	Coordinates2D coordinatesOne = coordinatesSystemOne.getCartesianCoordinates(), coordinatesTwo = coordinatesSystemTwo.getCartesianCoordinates();

	EXPECT_DOUBLE_EQ(coordinatesOne.x, 1.8867906598588415);
	EXPECT_DOUBLE_EQ(coordinatesOne.y, 2.5969445519435790);

	EXPECT_DOUBLE_EQ(coordinatesTwo.x, 12.827876867503392);
	EXPECT_DOUBLE_EQ(coordinatesTwo.y, -29.643474409592599);
}

TEST(CoordinatesSystemPolarTest, convertPolarToCartesian) {
	CoordinatesSystemPolar coordinatesSystemOne = CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(2.21 * M_PI), 23.21),
						   coordinatesSystemTwo = CoordinatesSystemPolar::fromAngleAndRadius(AngleInRadians(1.41 * M_PI), 8.23);

	Coordinates2D coordinatesOne = CoordinatesSystemPolar::convertPolarToCartesian(coordinatesSystemOne),
				  coordinatesTwo = CoordinatesSystemPolar::convertPolarToCartesian(coordinatesSystemTwo);

	EXPECT_DOUBLE_EQ(coordinatesOne.x, 18.339497837239804);
	EXPECT_DOUBLE_EQ(coordinatesOne.y, 14.225572715285544);

	EXPECT_DOUBLE_EQ(coordinatesTwo.x, -2.2960968027028525);
	EXPECT_DOUBLE_EQ(coordinatesTwo.y, -7.9032170331212424);
}

TEST(CoordinatesSystemPolarTest, convertCartesianToPolar) {
	CoordinatesSystemPolar coordinatesSystemOne = CoordinatesSystemPolar::convertCartesianToPolar({3.23, 4.53}),
						   coordinatesSystemTwo = CoordinatesSystemPolar::convertCartesianToPolar({6.32, -2.3}),
						   coordinatesSystemThree = CoordinatesSystemPolar::convertCartesianToPolar({322.33, 2.32}),
						   coordinatesSystemFour = CoordinatesSystemPolar::convertCartesianToPolar({34.33, 66.31});

	EXPECT_DOUBLE_EQ(coordinatesSystemOne.getRadius(), 5.56361393340695415);
	EXPECT_DOUBLE_EQ(coordinatesSystemOne.getAngleInRadians().getNormalizedRawAngle(), 0.951382611616222559);

	EXPECT_DOUBLE_EQ(coordinatesSystemTwo.getRadius(), 6.72550369860875179);
	EXPECT_DOUBLE_EQ(coordinatesSystemTwo.getAngleInRadians().getNormalizedRawAngle(), 5.93416023856049564);

	EXPECT_DOUBLE_EQ(coordinatesSystemThree.getRadius(), 322.338349099203522);
	EXPECT_DOUBLE_EQ(coordinatesSystemThree.getAngleInRadians().getNormalizedRawAngle(), 0.0071974682420346088);

	EXPECT_DOUBLE_EQ(coordinatesSystemFour.getRadius(), 74.6697060393303008);
	EXPECT_DOUBLE_EQ(coordinatesSystemFour.getAngleInRadians().getNormalizedRawAngle(), 1.09307357666142394);
}

TEST(CoordinatesSystemPolarTest, setLocationFrom2DCoordinates) {
	CoordinatesSystemPolar coordinatesSystem;
	coordinatesSystem.setLocationFrom2DCoordinates({34.33, 66.31});

	EXPECT_DOUBLE_EQ(coordinatesSystem.getRadius(), 74.6697060393303008);
	EXPECT_DOUBLE_EQ(coordinatesSystem.getAngleInRadians().getNormalizedRawAngle(), 1.09307357666142394);
}