#include <pebble.h>

#include "model.h"
#include "colors.h"

static Window *s_window;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;
static Layer *s_layer;

static TextLayer *s_title_layer;
static TextLayer *s_low_layer;
static TextLayer *s_medium_layer;
static TextLayer *s_high_layer;

static EnergyLevelChangedHandler s_energychanged_handler;
static EnergyLevel s_energy_level;


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_energy_level > 3)
    s_energy_level-=4;
  layer_mark_dirty(s_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_energy_level < ENERGY_LEVEL_MAX - 4)
    s_energy_level+=4;
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
  GRect bounds = layer_get_bounds(layer);
  bounds.size.h -= 10;
  bounds.size.h *= 2;
  bounds = grect_inset(bounds, GEdgeInsets(15));

  uint32_t angle = s_energy_level * 180 / ENERGY_LEVEL_MAX;
  GPoint point = gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(270 + angle));

  graphics_context_set_fill_color(ctx, getColorFromAngle(angle));
  graphics_fill_circle(ctx, point, 10);
  graphics_draw_circle(ctx, point, 10);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SPECTRUM);
  GRect layerbounds = gbitmap_get_bounds(s_bitmap);

  layerbounds.size.w += 10;
  layerbounds.size.h += 20;
  layerbounds.origin.x = (bounds.size.w - layerbounds.size.w) / 2;
  layerbounds.origin.y = (bounds.size.h - layerbounds.size.h) / 2;

  s_bitmap_layer = bitmap_layer_create(layerbounds);
  bitmap_layer_set_alignment(s_bitmap_layer, GAlignCenter);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));

  s_layer = layer_create(layerbounds);
  layer_set_update_proc(s_layer, layer_update_proc);
  layer_add_child(window_layer, s_layer);

  s_title_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(15,10), bounds.size.w, 30));
  text_layer_set_text_color(s_title_layer, GColorWhite);
  text_layer_set_background_color(s_title_layer, GColorClear);
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_text(s_title_layer, PBL_IF_ROUND_ELSE("What is your\ncurrent energy level ?","What is your current\nenergy level ?"));
  layer_add_child(window_layer, text_layer_get_layer(s_title_layer));

  s_low_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(25,5), layerbounds.origin.y + layerbounds.size.h - 10, bounds.size.w, 30));
  text_layer_set_text_color(s_low_layer, GColorWhite);
  text_layer_set_background_color(s_low_layer, GColorClear);
  text_layer_set_text_alignment(s_low_layer, GTextAlignmentLeft);
  text_layer_set_font(s_low_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_low_layer, "Exhausted");
  layer_add_child(window_layer, text_layer_get_layer(s_low_layer));

  s_medium_layer = text_layer_create(GRect(0, bounds.size.h / 2 - 15, bounds.size.w, 30));
  text_layer_set_text_color(s_medium_layer, GColorWhite);
  text_layer_set_background_color(s_medium_layer, GColorClear);
  text_layer_set_text_alignment(s_medium_layer, GTextAlignmentCenter);
  text_layer_set_font(s_medium_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_medium_layer, "Balanced\n& Alert");
  layer_add_child(window_layer, text_layer_get_layer(s_medium_layer));

  s_high_layer = text_layer_create(GRect(0, layerbounds.origin.y + layerbounds.size.h - 10, bounds.size.w - PBL_IF_ROUND_ELSE(25,5), 30));
  text_layer_set_text_color(s_high_layer, GColorWhite);
  text_layer_set_background_color(s_high_layer, GColorClear);
  text_layer_set_text_alignment(s_high_layer, GTextAlignmentRight);
  text_layer_set_font(s_high_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_high_layer, "Buzzing");
  layer_add_child(window_layer, text_layer_get_layer(s_high_layer));

}

static void window_appear(Window *window) {
  if( !s_bitmap ) {
  	s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SPECTRUM);
  	bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  }
}

static void window_disappear(Window *window) {
  gbitmap_destroy(s_bitmap);
  s_bitmap = NULL;
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(s_bitmap);
  layer_destroy(s_layer);
  text_layer_destroy(s_title_layer);
  text_layer_destroy(s_low_layer);
  text_layer_destroy(s_medium_layer);
  text_layer_destroy(s_high_layer);
  window_destroy(s_window);
}

void show_energy_window(EnergyLevel initial_level, EnergyLevelChangedHandler handler){
  s_energy_level = initial_level;
  s_energychanged_handler = handler;

  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .appear = window_appear,
    .disappear = window_disappear,
    .unload = window_unload,
  });
  window_set_click_config_provider(s_window, click_config_provider);
  window_stack_push(s_window, true);
}

void hide_energy_window() {
  window_stack_remove(s_window, true);
}
