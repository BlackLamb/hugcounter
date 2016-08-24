#include <pebble.h>
#include "enamel.h"
#include <pebble-events/pebble-events.h>
#include <@smallstoneapps/bitmap-loader/bitmap-loader.h>
#include <@smallstoneapps/font-loader/font-loader.h>

// Persistent Storage Keys, up the version if you change them
#define VERSION_PKEY 2
#define HUG_COUNT_PKEY 2

// Default Values
#define SET_HUGS_DEFAULT 1000

static Window *s_main_window;
static Layer *s_window_layer;
static TextLayer *s_clock_layer;
static TextLayer *s_hugs_layer;
static TextLayer *s_hugcount_layer;
static BitmapLayer *s_background_layer;
static int s_hug_count = 0;
static bool s_show_seconds;
static const uint8_t s_offset_top_percent = 24;
static const uint8_t s_offset_bottom_percent = 51;

static void update_time(struct tm* time_tick) {
	static char s_time_text[] = "00:00:00";
	
	if (s_show_seconds) {
		strftime(s_time_text, sizeof(s_time_text), clock_is_24h_style() ? "%H:%M:%S" : "%I:%M:%S", time_tick);
	} else {
		strftime(s_time_text, sizeof(s_time_text), clock_is_24h_style() ? "%H:%M" : "%I:%M", time_tick);
	}
	
	text_layer_set_text(s_clock_layer, s_time_text);
}

static void handle_time_tick(struct tm* time_tick, TimeUnits units_changed) {
	update_time(time_tick);
}

static void update_text() {
	static char s_hugs_left_text[5];
	int hugs_left = enamel_get_AppTotalHugsNum() - s_hug_count < 0 ? 0 : enamel_get_AppTotalHugsNum() - s_hug_count;
	snprintf(s_hugs_left_text, sizeof(s_hugs_left_text), "%u", hugs_left);
	text_layer_set_text(s_hugcount_layer, s_hugs_left_text);
}

static void increment_hug_click_handler(ClickRecognizerRef recognizer, void *context) {
	s_hug_count++;
	update_text();
}

static void reset_hug_count_click_handler(ClickRecognizerRef recognizer, void *context) {
	s_hug_count = 0;
	update_text();
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, increment_hug_click_handler);
	window_long_click_subscribe(BUTTON_ID_DOWN, 2000, reset_hug_count_click_handler, NULL);
}

static void setup_time_tick() {
	tick_timer_service_unsubscribe();
	if (s_show_seconds) {
		tick_timer_service_subscribe(SECOND_UNIT, handle_time_tick);
	} else {
		tick_timer_service_subscribe(MINUTE_UNIT, handle_time_tick);
	}
}

static void set_background_image() {
	uint8_t backgrounds[] = {
		RESOURCE_ID_IMAGE_HUGS_BACKGROUND,  //0
		RESOURCE_ID_IMAGE_HEART_BACKGROUND, //1
		RESOURCE_ID_IMAGE_HANDS_BACKGROUND  //2
	};
	
	bitmap_layer_set_bitmap(s_background_layer, bitmaps_get_bitmap_in_group(backgrounds[atoi(enamel_get_AppBackgroundV2())], 1));
}

static void prv_unobstructed_will_change(GRect final_unobstructed_screen_area, void *context) {
	// Get the full size of the screen
	GRect full_bounds = layer_get_bounds(s_window_layer);
	if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
		// Screen is about to become obstructed, hide the date
	layer_set_hidden(text_layer_get_layer(s_clock_layer), true);
	}
}

static void prv_unobstructed_did_change(void *context) {
	// Get the full size of the screen
	GRect full_bounds = layer_get_bounds(s_window_layer);
	// Get the total available screen real-estate
	GRect bounds = layer_get_unobstructed_bounds(s_window_layer);
	if (grect_equal(&full_bounds, &bounds)) {
		// Screen is no longer obstructed, show the date
		layer_set_hidden(text_layer_get_layer(s_clock_layer), false);
	}
}

uint8_t relative_pixel(int16_t percent, int16_t max) {
	return (max * percent) / 100;
}

static void prv_unobstructed_change(AnimationProgress progress, void *context) {
	// Get the total available screen real-estate
	GRect bounds = layer_get_unobstructed_bounds(s_window_layer);
	// Get the current position of our top text layer
	GRect frame = layer_get_frame(text_layer_get_layer(s_hugs_layer));
	// Shift the Y coordinate
	frame.origin.y = relative_pixel(s_offset_top_percent, bounds.size.h);
	// Apply the new location
	layer_set_frame(text_layer_get_layer(s_hugs_layer), frame);
	// Get the current position of our bottom text layer
	GRect frame2 = layer_get_frame(text_layer_get_layer(s_hugcount_layer));
	// Shift the Y coordinate
	frame2.origin.y = relative_pixel(s_offset_bottom_percent, bounds.size.h);
	// Apply the new position
	layer_set_frame(text_layer_get_layer(s_hugcount_layer), frame2);
}

static void main_window_load(Window *window) {
	s_window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(s_window_layer);
	GRect unob_bounds = layer_get_unobstructed_bounds(s_window_layer);
	// Set up layers
	s_background_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
	set_background_image();
	//bitmap_layer_set_bitmap(s_background_layer, bitmaps_get_bitmap(RESOURCE_ID_IMAGE_HUGS_BACKGROUND));
	
	s_clock_layer = text_layer_create(GRect(0, relative_pixel(6, unob_bounds.size.h), unob_bounds.size.w, 34));
	text_layer_set_text_color(s_clock_layer, GColorWhite);
	text_layer_set_background_color(s_clock_layer, GColorClear);
	text_layer_set_font(s_clock_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_clock_layer, GTextAlignmentCenter);
	
	s_hugs_layer = text_layer_create(GRect(0, relative_pixel(s_offset_top_percent, unob_bounds.size.h), unob_bounds.size.w, 34));
	text_layer_set_text_color(s_hugs_layer, GColorWhite);
	text_layer_set_background_color(s_hugs_layer, GColorClear);
	text_layer_set_font(s_hugs_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_hugs_layer, GTextAlignmentCenter);
	
	s_hugcount_layer = text_layer_create(GRect(0, relative_pixel(s_offset_bottom_percent, unob_bounds.size.h), unob_bounds.size.w, 50));
#ifdef PBL_COLOR
	text_layer_set_text_color(s_hugcount_layer, GColorRed);
	text_layer_set_font(s_hugcount_layer, fonts_get_font(RESOURCE_ID_FONT_CABINSKETCH_BOLD_48));
#else
	text_layer_set_text_color(s_hugcount_layer, GColorWhite);
	text_layer_set_font(s_hugcount_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
#endif
	text_layer_set_background_color(s_hugcount_layer, GColorClear);
	
	text_layer_set_text_alignment(s_hugcount_layer, GTextAlignmentCenter);
	
	// Set Default Content
	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	handle_time_tick(current_time, SECOND_UNIT);
	
	text_layer_set_text(s_hugs_layer, "HUGS LEFT:");
	
	update_text();
	
	// Subscriptions
	setup_time_tick();
	
	// Add Layers
	layer_add_child(s_window_layer, bitmap_layer_get_layer(s_background_layer));
	layer_add_child(s_window_layer, text_layer_get_layer(s_clock_layer));
	layer_add_child(s_window_layer, text_layer_get_layer(s_hugs_layer));
	layer_add_child(s_window_layer, text_layer_get_layer(s_hugcount_layer));
	
	// UnobstructedArea Stuff
	UnobstructedAreaHandlers handlers = {
		.will_change = prv_unobstructed_will_change,
    	.did_change = prv_unobstructed_did_change,
    	.change = prv_unobstructed_change
  	};
  	unobstructed_area_service_subscribe(handlers, NULL);
}

static void main_window_unload(Window *window) {
	tick_timer_service_unsubscribe();
	text_layer_destroy(s_clock_layer);
	text_layer_destroy(s_hugs_layer);
	text_layer_destroy(s_hugcount_layer);
	bitmap_layer_destroy(s_background_layer);
	layer_destroy(s_window_layer);
}

static void enamel_register_settings_received_cb(){	
	if (enamel_get_AppResetOnSave()) {
		s_hug_count = 0;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Resetting Count");
	}
	
	if (enamel_get_AppShowSeconds() != s_show_seconds) {
		s_show_seconds = enamel_get_AppShowSeconds();
		setup_time_tick();
	}
	
	set_background_image();
	
	update_text();
}

void handle_init(void) {
	enamel_init();
	bitmaps_init();
	fonts_init();
	s_hug_count = persist_exists(HUG_COUNT_PKEY) ? persist_read_int(HUG_COUNT_PKEY) : 0;
	s_show_seconds = enamel_get_AppShowSeconds();
	
	s_main_window = window_create();
	window_set_background_color(s_main_window, GColorBlack);
	window_set_click_config_provider(s_main_window, click_config_provider);
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	window_stack_push(s_main_window, true);
	enamel_register_settings_received(enamel_register_settings_received_cb);
	events_app_message_open(); 
}

void handle_deinit(void) {
	persist_write_int(HUG_COUNT_PKEY, s_hug_count);
	window_destroy(s_main_window);
	fonts_cleanup();
	bitmaps_cleanup();
	enamel_deinit();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
