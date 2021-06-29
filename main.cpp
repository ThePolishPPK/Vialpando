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

int main() {
	al_init();
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();
	// al_install_touch_input();
	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	ALLEGRO_DISPLAY *window = al_create_display(800, 600);
	ALLEGRO_EVENT_QUEUE *eventQueue = al_create_event_queue();
	al_register_event_source(eventQueue, al_get_display_event_source(window));
	al_register_event_source(eventQueue, al_get_mouse_event_source());
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	// al_register_event_source(eventQueue, al_get_touch_input_event_source());

	ImGui::CreateContext();
	ImGuiIO &IO = ImGui::GetIO();
	IO.IniFilename = NULL;
	ImGui_ImplAllegro5_Init(window);

	bool runned = true;
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