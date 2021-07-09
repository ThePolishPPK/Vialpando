project("Vialpando"
	LANGUAGES "CXX"
)
cmake_minimum_required(VERSION 3.16)

set(ImGuiHeaders "/usr/include/imgui/")

find_library(libImGui
	NAMES imgui ImGui DearImGui
	REQUIRED
)
find_library(libSTB
	NAMES stb STB
	REQUIRED
)
find_library(libAllegro
	NAMES allegro
	REQUIRED
)
find_library(libAllegro_video
	NAMES allegro_video
	REQUIRED
)
find_library(libAllegro_ttf
	NAMES allegro_ttf
	REQUIRED
)
find_library(libAllegro_primitives
	NAMES allegro_primitives
	REQUIRED
)
find_library(libAllegro_physfs
	NAMES allegro_physfs
	REQUIRED
)
find_library(libAllegro_memfile
	NAMES allegro_memfile
	REQUIRED
)
find_library(libAllegro_main
	NAMES allegro_main
	REQUIRED
)
find_library(libAllegro_image
	NAMES allegro_image
	REQUIRED
)
find_library(libAllegro_font
	NAMES allegro_font
	REQUIRED
)
find_library(libAllegro_dialog
	NAMES allegro_dialog
	REQUIRED
)
find_library(libAllegro_color
	NAMES allegro_color
	REQUIRED
)
find_library(libAllegro_audio
	NAMES allegro_audio
	REQUIRED
)
find_library(libAllegro_acodec
	NAMES allegro_acodec
	REQUIRED
)

include_directories(
	${ImGuiHeaders}
)

add_executable(${PROJECT_NAME}
	main.cpp
	menu.cpp
	Simulations/gravity.cpp
)

target_link_libraries(${PROJECT_NAME}
	${libImGui}
	${libSTB}
	${libAllegro}
	${libAllegro_video}
	${libAllegro_ttf}
	${libAllegro_primitives}
	${libAllegro_physfs}
	${libAllegro_memfile}
	${libAllegro_main}
	${libAllegro_image}
	${libAllegro_font}
	${libAllegro_dialog}
	${libAllegro_color}
	${libAllegro_audio}
	${libAllegro_acodec}
)