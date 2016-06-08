#include "model.h"
#include "constants.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;

static GBitmap *s_checked_bitmap, *s_unchecked_bitmap;
static bool s_selections[NB_OF_ACTIVITIES];

static DictationSession *s_dictation_session = NULL;
static char s_other[MAX_TEXT_LENGTH];

static ActivityChangedHandler s_handler;

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    s_selections[NB_OF_ACTIVITIES - 1] = true;
    snprintf(s_other, sizeof(s_other), "%s", transcription);
    APP_LOG(0, "text : %s", s_other);
    menu_layer_reload_data(s_menu_layer);
  } 
  if(s_dictation_session){
    dictation_session_destroy(s_dictation_session);
    s_dictation_session = NULL;
  }
}

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NB_OF_ACTIVITIES + 1;
}

static void draw_header_callback(GContext *ctx, const Layer *cell_layer,  uint16_t section_index, void *context) {
  menu_cell_basic_header_draw(ctx, cell_layer, PBL_IF_ROUND_ELSE("         Past hour activities","Past hour activities"));
}

static int16_t get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  if(cell_index->row == NB_OF_ACTIVITIES) {
    // Submit item
    menu_cell_basic_draw(ctx, cell_layer, "Submit", NULL, NULL);
  } else if(cell_index->row == NB_OF_ACTIVITIES - 1 && s_other[0]) {
    // Submit item
    menu_cell_basic_draw(ctx, cell_layer, s_other, NULL, s_selections[cell_index->row] ? s_checked_bitmap : s_unchecked_bitmap);
  } else {
    menu_cell_basic_draw(ctx, cell_layer, ACTIVITY_TEXTS[cell_index->row], NULL, s_selections[cell_index->row] ? s_checked_bitmap : s_unchecked_bitmap);
  }
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  int row = cell_index->row;
  if(row == NB_OF_ACTIVITIES) {
    if(s_handler) {
      s_handler(s_selections, s_other);
    }
  } else if(row == NB_OF_ACTIVITIES-1){
    if(s_selections[row]) {
      s_selections[row] = false;
      s_other[0] = 0;
    }
    else {
      if(!s_dictation_session)
        s_dictation_session = dictation_session_create(sizeof(s_other), dictation_session_callback, NULL);
      dictation_session_start(s_dictation_session);
    }
  } else {
    // Check/uncheck
    s_selections[row] = !s_selections[row];
    menu_layer_reload_data(menu_layer);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_checked_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SELECT);
  s_unchecked_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UNCHECKED);

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

  gbitmap_destroy(s_checked_bitmap);
  gbitmap_destroy(s_unchecked_bitmap);

  if(s_dictation_session){
    dictation_session_destroy(s_dictation_session);
    s_dictation_session = NULL;
  }

  window_destroy(window);
  s_main_window = NULL;
}

void show_activity_window(ActivityChangedHandler handler) {
  memset(s_selections, 0, NB_OF_ACTIVITIES * sizeof(bool));
  s_handler = handler;
  s_other[0] = 0;

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
}

void hide_activity_window(){
  window_stack_remove(s_main_window, true);
}