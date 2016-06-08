/**
 * Example implementation of the dialog choice UI pattern.
 */

#include "dialog_choice_window.h"
#include "model.h"

static Window *s_main_window = NULL;
static TextLayer *s_label_layer;
static BitmapLayer *s_icon_layer = NULL;
static ActionBarLayer *s_action_bar_layer;

static const char* s_message;
static uint32_t s_image_id;
static GBitmap *s_icon_bitmap = NULL;
static uint32_t s_button_image_id[3];
static GBitmap* s_button_bitmap[3];
static ActionHandler s_button_handlers[3];

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_button_handlers[0]){
    window_stack_remove(s_main_window, true);
    s_button_handlers[0]();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_button_handlers[1]){
    window_stack_remove(s_main_window, true);
    s_button_handlers[1]();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(s_button_handlers[2]){
    window_stack_remove(s_main_window, true);
    s_button_handlers[2]();
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP,up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT,select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN,down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  uint8_t y_offset = 5;

  if(s_image_id){
    s_icon_bitmap = gbitmap_create_with_resource(s_image_id);
    GRect bitmap_bounds = gbitmap_get_bounds(s_icon_bitmap);

    s_icon_layer = bitmap_layer_create(PBL_IF_ROUND_ELSE(
        GRect((bounds.size.w - bitmap_bounds.size.w) / 2, ACTION_BAR_WIDTH / 2, bitmap_bounds.size.w, bitmap_bounds.size.h),
        GRect((bounds.size.w - ACTION_BAR_WIDTH - bitmap_bounds.size.w) / 2, ACTION_BAR_WIDTH / 4, bitmap_bounds.size.w, bitmap_bounds.size.h)
    ));
    bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
    bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_icon_layer));

    y_offset += bitmap_bounds.size.h + PBL_IF_ROUND_ELSE(20, 0);
  }

  GEdgeInsets label_insets = {.top = y_offset, .right = ACTION_BAR_WIDTH + 10, .left = 10};
  s_label_layer = text_layer_create(grect_inset(bounds, label_insets));
  text_layer_set_text(s_label_layer, s_message);
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text_alignment(s_label_layer, PBL_IF_ROUND_ELSE(GTextAlignmentRight, GTextAlignmentCenter));
  text_layer_set_font(s_label_layer, strlen(s_message) > 50 ? fonts_get_system_font(FONT_KEY_GOTHIC_18) : fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  for(uint8_t i=0; i<3; i++){
    s_button_bitmap[i] = s_button_image_id[i] ? gbitmap_create_with_resource(s_button_image_id[i]) : 0;
  }

  s_action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP,     s_button_bitmap[0]);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_SELECT, s_button_bitmap[1]);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_DOWN,   s_button_bitmap[2]);
  action_bar_layer_add_to_window(s_action_bar_layer, window);
  action_bar_layer_set_click_config_provider(s_action_bar_layer, click_config_provider);
}

static void window_unload(Window *window) {
  text_layer_destroy(s_label_layer);
  action_bar_layer_destroy(s_action_bar_layer);
  if(s_icon_layer)
    bitmap_layer_destroy(s_icon_layer);
  s_icon_layer = NULL;

  if(s_icon_bitmap)
    gbitmap_destroy(s_icon_bitmap);
  s_icon_bitmap = NULL;

  for(uint8_t i=0; i<3; i++){
    if(s_button_bitmap[i])
      gbitmap_destroy(s_button_bitmap[i]);
  }

  window_destroy(window);
  s_main_window = NULL;
}

void dialog_choice_window_push(const char* message, uint32_t image_id, ActionHandler up_handler, uint32_t up_image_id, ActionHandler select_handler, uint32_t select_image_id, ActionHandler down_handler, uint32_t down_image_id) {
  if(!s_main_window) {
    s_message = message;
    s_image_id = image_id;

    s_button_image_id[0] = up_image_id;
    s_button_image_id[1] = select_image_id;
    s_button_image_id[2] = down_image_id;

    s_button_handlers[0] = up_handler;
    s_button_handlers[1] = select_handler;
    s_button_handlers[2] = down_handler;

    s_main_window = window_create();
    window_set_background_color(s_main_window, PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorWhite));
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}