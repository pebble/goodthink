#include <pebble.h>

#include "model.h"
#include "constants.h"

static Window         *window;

static ActionBarLayer *action_bar;
static GBitmap *up_icon;
static GBitmap *down_icon;
static GBitmap *select_icon;

static Layer* s_bg_layer;
static GPath* s_arrow_path;
static GBitmap *s_bitmap;

static TextLayer *mood_text_layer;

static MoodChangedHandler s_moodchanged_handler;
static MoodLevel s_mood_level;


static const GPathInfo ARROW_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{7, 0}, {2, 50}, {-2, 50}, {-7, 0}}
};

static void bg_update_proc(struct Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  graphics_draw_bitmap_in_rect(ctx, s_bitmap, gbitmap_get_bounds(s_bitmap));

  gpath_rotate_to(s_arrow_path, DEG_TO_TRIGANGLE(s_mood_level * 180 / 100));

  // Fill the path:
  graphics_context_set_fill_color(ctx, GColorRed);
  gpath_draw_filled(ctx, s_arrow_path);
  // Stroke the path:
  graphics_context_set_stroke_width(ctx, 3);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, s_arrow_path);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, (GPoint){PBL_IF_ROUND_ELSE(100,89), bounds.size.h/2}, 32);
  graphics_fill_rect(ctx, (GRect){.origin={PBL_IF_ROUND_ELSE(100,89),bounds.size.h/2-32},.size={64,65}}, 0, 0);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_mood_level < MOOD_LEVEL_MAX - 4)
    s_mood_level+=4;
  layer_mark_dirty(s_bg_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_mood_level > 3)
    s_mood_level-=4;
  layer_mark_dirty(s_bg_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_moodchanged_handler) {
    s_moodchanged_handler(s_mood_level);
  }
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP,50,up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN,50,down_click_handler);
}

static void cb_load(Window *window) {  
  // The bitmap layer holds the image for display
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  mood_text_layer = text_layer_create(GRect(0, bounds.size.h / 2 - 23, bounds.size.w - PBL_IF_ROUND_ELSE(48,30), 14*3));
  text_layer_set_text_color(mood_text_layer, GColorWhite);
  text_layer_set_background_color(mood_text_layer, GColorClear);
  text_layer_set_text_alignment(mood_text_layer, GTextAlignmentRight);
  text_layer_set_font(mood_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(mood_text_layer, "SELECT\nCURRENT\nMOOD");
  layer_add_child(window_layer, text_layer_get_layer(mood_text_layer));

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

static void cb_unload(Window *window) {
  // Destroy the action bar
  action_bar_layer_destroy(action_bar);

  // Destroy the text layers
  text_layer_destroy(mood_text_layer);

  layer_destroy(s_bg_layer);

  // Destroy the action bar icons
  gbitmap_destroy(up_icon);
  gbitmap_destroy(down_icon);
  gbitmap_destroy(select_icon);

  // destroy window
  window_destroy(window);
}

static void cb_appear(Window *window) {
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOODS);
  s_arrow_path = gpath_create(&ARROW_PATH_INFO);
  gpath_move_to(s_arrow_path, GPoint(PBL_IF_ROUND_ELSE(94,78),PBL_IF_ROUND_ELSE(90,84)));
}

static void cb_disappear(Window *window) {
  gpath_destroy(s_arrow_path);
  gbitmap_destroy(s_bitmap);
}

void show_mood_window(MoodLevel initial_level, MoodChangedHandler handler) {
  s_mood_level = initial_level;
  s_moodchanged_handler = handler;

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load      = cb_load,
    .unload    = cb_unload,
    .appear    = cb_appear,
    .disappear    = cb_disappear
  });

  window_stack_push(window, true);
}

void hide_mood_window() {
  window_stack_remove(window, true);
}