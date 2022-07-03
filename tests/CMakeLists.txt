project("VialpandoTest"
	LANGUAGES "CXX"
)
cmake_minimum_required(VERSION 3.13)

add_executable("${PROJECT_NAME}"
	mechanisms/coordinates.cpp
	mechanisms/angle.cpp
)

target_link_libraries("${PROJECT_NAME}"
	"Vialpando"
	gtest_main
	gtest
)