#include <pebble.h>

#include "model.h"
#include "constants.h"
#include "colors.h"

static Window         *window;

static ActionBarLayer *action_bar;
static GBitmap *up_icon;
static GBitmap *down_icon;
static GBitmap *select_icon;

static Layer* s_bg_layer;
static GPath* s_arrow_path;
static GBitmap *mood_icon[NB_OF_MOODS];

static TextLayer *mood_text_layer;

static MoodChangedHandler s_moodchanged_handler;
static MoodLevel s_mood_level;


static const GPathInfo ARROW_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{7, 0}, {0, 14}, {-7, 0}}
};

static void bg_update_proc(struct Layer *layer, GContext *ctx){
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  GRect bounds = layer_get_bounds(layer);
  bounds = grect_inset(bounds, GEdgeInsets(20));
  for(uint8_t i=0; i<NB_OF_MOODS; i++){
    GPoint p = gpoint_from_polar(bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(180 + i * 45));
    GRect bounds = gbitmap_get_bounds(mood_icon[i]);
    p.x -= bounds.size.w/2;
    p.y -= bounds.size.h/2;
    bounds.origin = p;
    graphics_draw_bitmap_in_rect(ctx, mood_icon[i], bounds);
  }

  bounds = grect_inset(bounds, GEdgeInsets(27));
  uint8_t angle = s_mood_level * 180 / 100;
  GPoint p = gpoint_from_polar(bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(180 + angle));
  gpath_move_to(s_arrow_path, p);
  gpath_rotate_to(s_arrow_path, DEG_TO_TRIGANGLE(angle));

  gpath_draw_filled(ctx, s_arrow_path);
}

static void update_mood() {  
  uint8_t angle = s_mood_level * 180 / 100;
  window_set_background_color(window, getColorFromAngle(angle));
  layer_mark_dirty(s_bg_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_mood_level < MOOD_LEVEL_MAX - 4)
    s_mood_level+=4;
  update_mood();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_mood_level > 3)
    s_mood_level-=4;
  update_mood();
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
  // Initialize the action bar:
  action_bar = action_bar_layer_create();
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

  // Change the background color
  action_bar_layer_set_background_color(action_bar, GColorWhite);
  // Set the icons:
  up_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP);
  down_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN);
  select_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TICK2);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP,     up_icon);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN,   down_icon);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, select_icon);

  // load current mood icon
  for(uint8_t i=0; i<NB_OF_MOODS; i++){
    mood_icon[i] = gbitmap_create_with_resource(MOOD_BIG_RESOURCE_IDS[i]);
  }
  
  // The bitmap layer holds the image for display
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  mood_text_layer = text_layer_create(GRect(0, bounds.size.h/2 - 42, bounds.size.w - ACTION_BAR_WIDTH - PBL_IF_ROUND_ELSE(15,5), 80));
  text_layer_set_font(mood_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(mood_text_layer, "Select\nCurrent\nMood");
  text_layer_set_text_alignment(mood_text_layer, GTextAlignmentRight);
  text_layer_set_text_color(mood_text_layer, GColorBlack);
  text_layer_set_background_color(mood_text_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(mood_text_layer));

  bounds.size.w = bounds.size.h;

  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  s_arrow_path = gpath_create(&ARROW_PATH_INFO);

  update_mood();
}

static void cb_unload(Window *window) {

  layer_destroy(s_bg_layer);
  gpath_destroy(s_arrow_path);

  // Destroy the action bar
  action_bar_layer_destroy(action_bar);

  // // Destroy the text layers
  text_layer_destroy(mood_text_layer);

  // Destroy the action bar icons
  gbitmap_destroy(up_icon);
  gbitmap_destroy(down_icon);
  gbitmap_destroy(select_icon);

  // Destroy the mood icon
  for(uint8_t i=0; i<NB_OF_MOODS; i++){
    gbitmap_destroy(mood_icon[i]);
  }

  // destroy window
  window_destroy(window);
}

void show_mood_window(MoodLevel initial_level, MoodChangedHandler handler) {
  s_mood_level = initial_level;
  s_moodchanged_handler = handler;

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load      = cb_load,
    .unload    = cb_unload
  });

  window_stack_push(window, true);
}

void hide_mood_window() {
  window_stack_remove(window, true);
}