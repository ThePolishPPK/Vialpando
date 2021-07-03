#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/mouse.h>
#include <allegro5/system.h>
#include <allegro5/touch_input.h>
#include <backends/imgui_impl_allegro5.h>
#include <imgui.h>

#include "menu.hpp"

int main() {
	bool isTouch = al_is_touch_input_installed();
	al_init();
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();
	if (isTouch) al_install_touch_input();
	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	ALLEGRO_DISPLAY *window = al_create_display(800, 600);
	ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();
	al_register_event_source(eventQueue, al_get_display_event_source(window));
	al_register_event_source(eventQueue, al_get_mouse_event_source());
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	if (isTouch)
		al_register_event_source(eventQueue, al_get_touch_input_event_source());

	ImGui::CreateContext();
	ImWchar glyphRange[] = {0x0020, 0xFFFF};
	ImGuiIO &IO = ImGui::GetIO();
	IO.IniFilename = NULL;
	IO.Fonts->AddFontFromFileTTF("./Roboto.ttf", 18, 0, (ImWchar *)&glyphRange);
	IO.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui_ImplAllegro5_Init(window);

	bool runned = true;
	Menu menu(IO);
	while (runned) {
		ALLEGRO_EVENT event;
		while (al_get_next_event(eventQueue, &event)) {
			ImGui_ImplAllegro5_ProcessEvent(&event);
			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				runned = false;
			}
			if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
				ImGui_ImplAllegro5_InvalidateDeviceObjects();
				al_acknowledge_resize(window);
				ImGui_ImplAllegro5_CreateDeviceObjects();
			}
		}

		ImGui_ImplAllegro5_NewFrame();
		ImGui::NewFrame();

		if (menu.keepActive == false) {
			runned = false;
		} else
			menu.draw();

		ImGui::Render();
		al_clear_to_color(al_map_rgb(0, 0, 0));
		ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
		al_flip_display();
	}
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	al_destroy_display(window);
	al_destroy_event_queue(eventQueue);
	return 0;
}