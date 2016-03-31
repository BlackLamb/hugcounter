#include <pebble.h>

// Persistent Storage Keys, up the version if you change them
#define VERSION_PKEY 1
#define HUG_COUNT_PKEY 2
#define SET_HUGS_PKEY 3

// Default Values
#define SET_HUGS_DEFAULT 1000

// Matching values to those in 'Settings'
typedef enum {
	AppTotalHugsNum = 0,
	AppResetOnSave = 1
} AppKey;

Window *s_main_window;
TextLayer *s_clock_layer;
TextLayer *s_hugs_layer;
TextLayer *s_hugcount_layer;
GFont s_cabinsketch_bold_48;
GBitmap *s_background;
BitmapLayer *s_background_layer;

int s_hug_count = 0;
int s_set_hugs = SET_HUGS_DEFAULT;

static void handle_second_tick(struct tm* time_tick, TimeUnits units_changed) {
	static char s_time_text[] = "00:00:00";
	
	strftime(s_time_text, sizeof(s_time_text), clock_is_24h_style() ? "%H:%M:%S" : "%I:%M:%S", time_tick);
	text_layer_set_text(s_clock_layer, s_time_text);
}

static void update_text() {
	static char s_hugs_left_text[5];
	int hugs_left = s_set_hugs - s_hug_count < 0 ? 0 : s_set_hugs - s_hug_count;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Hugs Math %u - %u = %u", s_set_hugs, s_hug_count, hugs_left);
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

static void main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	s_cabinsketch_bold_48 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CABINSKETCH_BOLD_48));
	s_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HUGS_BACKGROUND);
	
	// Set up layers
	s_background_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
	bitmap_layer_set_bitmap(s_background_layer, s_background);
	
	s_clock_layer = text_layer_create(GRect(0, 10, bounds.size.w, 34));
	text_layer_set_text_color(s_clock_layer, GColorWhite);
	text_layer_set_background_color(s_clock_layer, GColorClear);
	text_layer_set_font(s_clock_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_clock_layer, GTextAlignmentCenter);
	
	s_hugs_layer = text_layer_create(GRect(0, 40, bounds.size.w, 34));
	text_layer_set_text_color(s_hugs_layer, GColorWhite);
	text_layer_set_background_color(s_hugs_layer, GColorClear);
	text_layer_set_font(s_hugs_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(s_hugs_layer, GTextAlignmentCenter);
	
	s_hugcount_layer = text_layer_create(GRect(0, 85, bounds.size.w, 50));
	text_layer_set_text_color(s_hugcount_layer, GColorRed);
	text_layer_set_background_color(s_hugcount_layer, GColorClear);
	text_layer_set_font(s_hugcount_layer, s_cabinsketch_bold_48);
	text_layer_set_text_alignment(s_hugcount_layer, GTextAlignmentCenter);
	
	// Set Default Content
	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	handle_second_tick(current_time, SECOND_UNIT);
	
	text_layer_set_text(s_hugs_layer, "HUGS LEFT:");
	
	update_text();
	
	// Subscriptions
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
	
	// Add Layers
	layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_clock_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_hugs_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_hugcount_layer));
}

static void main_window_unload(Window *window) {
	tick_timer_service_unsubscribe();
	text_layer_destroy(s_clock_layer);
	text_layer_destroy(s_hugs_layer);
	text_layer_destroy(s_hugcount_layer);
	fonts_unload_custom_font(s_cabinsketch_bold_48);
	gbitmap_destroy(s_background);
	bitmap_layer_destroy(s_background_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
	bool changeCount = false;
	
	Tuple *set_hugs_t = dict_find(iter, AppTotalHugsNum);
	if(set_hugs_t && set_hugs_t->value->int32 != s_set_hugs) {
		s_set_hugs = set_hugs_t->value->int32;
		if(s_set_hugs > 9999) {
			s_set_hugs = 9999;
		}
		changeCount = true;
	}
	
	Tuple *reset_on_save_t = dict_find(iter, AppResetOnSave);
	if(reset_on_save_t && changeCount) {
		bool resetOnSave = reset_on_save_t->value->int32 == 1;
		if(resetOnSave){
			s_hug_count = 0;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Resetting Count");
		}
	}
	
	persist_write_int(SET_HUGS_PKEY, s_set_hugs);
	update_text();
}

void handle_init(void) {
	s_hug_count = persist_exists(HUG_COUNT_PKEY) ? persist_read_int(HUG_COUNT_PKEY) : 0;
	s_set_hugs = persist_exists(SET_HUGS_PKEY) ? persist_read_int(SET_HUGS_PKEY) : SET_HUGS_DEFAULT;
	
	s_main_window = window_create();
	window_set_background_color(s_main_window, GColorBlack);
	window_set_click_config_provider(s_main_window, click_config_provider);
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	window_stack_push(s_main_window, true);
	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(255,255);
}

void handle_deinit(void) {
	persist_write_int(HUG_COUNT_PKEY, s_hug_count);
	app_message_deregister_callbacks();
	window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
