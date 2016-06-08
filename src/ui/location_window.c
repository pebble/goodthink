#include "model.h"
#include "constants.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;

static DictationSession *s_dictation_session = NULL;
static char s_other[MAX_TEXT_LENGTH];

static LocationChangedHandler s_handler;

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    snprintf(s_other, sizeof(s_other), "%s", transcription);
    APP_LOG(0, "text : %s", s_other);
    s_handler(NB_OF_LOCATIONS-1, s_other);
  } 
  if(s_dictation_session){
    dictation_session_destroy(s_dictation_session);
    s_dictation_session = NULL;
  }
}

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NB_OF_LOCATIONS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  menu_cell_basic_draw(ctx, cell_layer, LOCATION_TEXTS[cell_index->row], NULL, NULL);
}

static int16_t get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void draw_header_callback(GContext *ctx, const Layer *cell_layer,  uint16_t section_index, void *context) {
  menu_cell_basic_header_draw(ctx, cell_layer, PBL_IF_ROUND_ELSE("               Where are you?","Where are you?"));
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if(cell_index->row == NB_OF_LOCATIONS-1){
    if(!s_dictation_session)
      s_dictation_session = dictation_session_create(sizeof(s_other), dictation_session_callback, NULL);
    dictation_session_start(s_dictation_session);
  }
  else if(s_handler) {
    s_handler(cell_index->row, s_other);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .draw_header = draw_header_callback,
      .get_header_height = get_header_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);

  if(s_dictation_session){
    dictation_session_destroy(s_dictation_session);
    s_dictation_session = NULL;
  }

  window_destroy(window);
  s_main_window = NULL;
}

void show_location_window(LocationChangedHandler handler) {
  s_handler = handler;
  s_other[0] = 0;

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
}

void hide_location_window(){
  window_stack_remove(s_main_window, true);
}