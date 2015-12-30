#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_day_layer, *s_date_layer, *s_time_layer;


static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char s_day_text[] = "Xxxxxxxxxx";
  static char s_time_text[] = "00:00";
  static char s_date_text[] = "Xxxxxxxxx 00";

  strftime(s_day_text, sizeof(s_date_text), "%A", tick_time);
  text_layer_set_text(s_day_layer, s_day_text);

  clock_copy_time_string(s_time_text, sizeof(s_time_text));
  text_layer_set_text(s_time_layer, s_time_text);

  strftime(s_date_text, sizeof(s_date_text), "%B %e", tick_time);
  text_layer_set_text(s_date_layer, s_date_text);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  const int day_height = 30;
  const int time_height = 55;
  const int date_height = 30;

  s_day_layer = text_layer_create(GRect(0, 0, bounds.size.w, day_height));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));

  s_time_layer = text_layer_create(GRect(0, (bounds.size.h / 2) - (time_height/2), bounds.size.w, time_height));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  s_date_layer = text_layer_create(GRect(0, bounds.size.h - date_height, bounds.size.w, date_height));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_main_window, animated);
  
  setlocale(LC_ALL, "");
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  
  // Prevent starting blank
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  handle_minute_tick(t, MINUTE_UNIT);
}

static void deinit() {
  window_destroy(s_main_window);

  tick_timer_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
