#include <pebble.h>

#include "model.h"

static Window *s_window;
static GBitmap *s_bitmap;
static Layer *s_layer;

static TextLayer *s_title_layer;
static TextLayer *s_low_layer;
static TextLayer *s_medium_layer;
static TextLayer *s_high_layer;

static EnergyLevelChangedHandler s_energychanged_handler;
static EnergyLevel s_energy_level;

static ActionBarLayer *action_bar;
static GBitmap *up_icon;
static GBitmap *down_icon;
static GBitmap *select_icon;

static const GPathInfo ARROW_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{5, 0}, {3, 85}, {-3, 85}, {-5, 0}}
};
static GPath *s_my_path_ptr = NULL;


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_energy_level < ENERGY_LEVEL_MAX - 4)
    s_energy_level+=4;
  layer_mark_dirty(s_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_energy_level > 3)
    s_energy_level-=4;
  layer_mark_dirty(s_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_energychanged_handler) {
  	s_energychanged_handler(s_energy_level);
  }
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP,50,up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN,50,down_click_handler);
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
#ifdef PBL_ROUND 
  graphics_draw_bitmap_in_rect(ctx, s_bitmap, (GRect){.origin={73,10},.size={73,160}});
  gpath_rotate_to(s_my_path_ptr, DEG_TO_TRIGANGLE(25 + s_energy_level * 130 / ENERGY_LEVEL_MAX));
#else
  graphics_draw_bitmap_in_rect(ctx, s_bitmap, (GRect){.origin={51,3},.size={63,162}});
  gpath_rotate_to(s_my_path_ptr, DEG_TO_TRIGANGLE(15 + s_energy_level * 150 / ENERGY_LEVEL_MAX));
#endif
  

  // Fill the path:
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, s_my_path_ptr);
  // Stroke the path:
  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, s_my_path_ptr);

  graphics_context_set_fill_color(ctx, GColorBlack);
#ifdef PBL_ROUND 
  graphics_fill_circle(ctx, (GPoint){157, 90}, 67);
#else
  graphics_fill_circle(ctx, (GPoint){133, 84}, 70);
#endif
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, layer_update_proc);
  layer_add_child(window_layer, s_layer);

  s_low_layer = text_layer_create(GRect(0, bounds.size.h - PBL_IF_ROUND_ELSE(30,20), bounds.size.w - PBL_IF_ROUND_ELSE(20,30), 30));
  text_layer_set_text_color(s_low_layer, GColorBlack);
  text_layer_set_background_color(s_low_layer, GColorClear);
  text_layer_set_text_alignment(s_low_layer, GTextAlignmentCenter);
  text_layer_set_font(s_low_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_low_layer, "Exhausted");
  layer_add_child(window_layer, text_layer_get_layer(s_low_layer));

  s_medium_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(15,2), bounds.size.h / 2 - 15, bounds.size.w - 30, 30));
  text_layer_set_text_color(s_medium_layer, GColorBlack);
  text_layer_set_background_color(s_medium_layer, GColorClear);
  text_layer_set_text_alignment(s_medium_layer, GTextAlignmentLeft);
  text_layer_set_font(s_medium_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_medium_layer, "Balanced\n& Alert");
  layer_add_child(window_layer, text_layer_get_layer(s_medium_layer));

  s_high_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(10,0), bounds.size.w - PBL_IF_ROUND_ELSE(15,30), 30));
  text_layer_set_text_color(s_high_layer, GColorBlack);
  text_layer_set_background_color(s_high_layer, GColorClear);
  text_layer_set_text_alignment(s_high_layer, GTextAlignmentCenter);
  text_layer_set_font(s_high_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_high_layer, "Buzzing");
  layer_add_child(window_layer, text_layer_get_layer(s_high_layer));

  s_title_layer = text_layer_create(GRect(0, bounds.size.h / 2 - 23, bounds.size.w - PBL_IF_ROUND_ELSE(40,30), 14*3));
  text_layer_set_text_color(s_title_layer, GColorWhite);
  text_layer_set_background_color(s_title_layer, GColorClear);
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentRight);
  text_layer_set_font(s_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(s_title_layer, "SELECT\nENERGY\nLEVEL");
  layer_add_child(window_layer, text_layer_get_layer(s_title_layer));


  // Initialize the action bar:
  action_bar = action_bar_layer_create();
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

  // Change the background color
  action_bar_layer_set_background_color(action_bar, GColorBlack);
  // Set the icons:
  up_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP2);
  down_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN2);
  select_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TICK);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP,     up_icon);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN,   down_icon);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, select_icon);
}

static void window_appear(Window *window) {
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SPECTRUM);
  s_my_path_ptr = gpath_create(&ARROW_PATH_INFO);
  gpath_move_to(s_my_path_ptr, PBL_IF_ROUND_ELSE(GPoint(156,90),GPoint(132,84)));
}

static void window_disappear(Window *window) {
  gpath_destroy(s_my_path_ptr);
  gbitmap_destroy(s_bitmap);
}

static void window_unload(Window *window) {
  layer_destroy(s_layer);

  text_layer_destroy(s_title_layer);
  text_layer_destroy(s_low_layer);
  text_layer_destroy(s_medium_layer);
  text_layer_destroy(s_high_layer);

  // Destroy the action bar
  action_bar_layer_destroy(action_bar);

  // Destroy the action bar icons
  gbitmap_destroy(up_icon);
  gbitmap_destroy(down_icon);
  gbitmap_destroy(select_icon);

  window_destroy(s_window);
}

void show_energy_window(EnergyLevel initial_level, EnergyLevelChangedHandler handler){
  s_energy_level = initial_level;
  s_energychanged_handler = handler;

  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .appear = window_appear,
    .disappear = window_disappear,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_energy_window() {
  window_stack_remove(s_window, true);
}
