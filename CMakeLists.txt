project("Vialpando"
	LANGUAGES "CXX"
)
cmake_minimum_required(VERSION 3.13)


set(BUILD_SHARED_LIBS OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/tinygettext/)

include_directories(
	"${CMAKE_SOURCE_DIR}/imgui/"
	"${CMAKE_SOURCE_DIR}/imgui/backends/"
	"${CMAKE_SOURCE_DIR}/imgui/misc/cpp/"
	"${CMAKE_SOURCE_DIR}/imgui/misc/freetype/"
	"${CMAKE_SOURCE_DIR}/tinygettext/include/"
	"/usr/include/freetype2/"
	"/usr/include/stb/"
)

add_executable(${PROJECT_NAME}
	main.cpp
	menu.cpp
	basic.cpp
	translate.cpp
	Simulations/gravity.cpp
	Simulations/dynamic_law.cpp
	Simulations/work_and_energy.cpp
	Simulations/electric_field.cpp
	Simulations/electric_field_needle.cpp
)

add_library(ImGui_Allegro STATIC
	${CMAKE_SOURCE_DIR}/imgui/imgui_tables.cpp
	${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp
	${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp
	${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/imgui/imgui.cpp
	${CMAKE_SOURCE_DIR}/imgui/misc/freetype/imgui_freetype.cpp
	${CMAKE_SOURCE_DIR}/imgui/misc/cpp
	${CMAKE_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.cpp
	${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_allegro5.cpp
)

target_link_libraries(${PROJECT_NAME}
	ImGui_Allegro
	tinygettext
	stb
	allegro
	freetype
	allegro_video
	allegro_ttf
	allegro_primitives
	allegro_physfs
	allegro_memfile
	allegro_main
	allegro_image
	allegro_font
	allegro_dialog
	allegro_color
	allegro_audio
	allegro_acodec
)