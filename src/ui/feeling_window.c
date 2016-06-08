#include "model.h"
#include "constants.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;

static FeelingChangedHandler s_handler;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NB_OF_FEELINGS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  menu_cell_basic_draw(ctx, cell_layer, FEELING_TEXTS[cell_index->row], NULL, NULL);
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if(s_handler) {
    s_handler(cell_index->row);
  }
}

static int16_t get_header_height_callback(MenuLayer *ml, uint16_t section_index, void *ctx) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
  menu_cell_basic_header_draw(ctx, cell_layer, PBL_IF_ROUND_ELSE("             How do you feel?","How do you feel?")); //FIXME this is ugly
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .select_click = select_callback,
      .get_header_height = get_header_height_callback,
      .draw_header = draw_header_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);

  window_destroy(window);
  s_main_window = NULL;
}

void show_feeling_window(FeelingChangedHandler handler) {
  s_handler = handler;

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
}

void hide_feeling_window(){
  window_stack_remove(s_main_window, true);
}