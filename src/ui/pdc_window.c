#include <pebble.h>

// Milliseconds between frames - 30 FPS max
#define DELTA 33

static Window *s_main_window = NULL;
static Layer *s_canvas_layer = NULL;
static GDrawCommandSequence *s_command_seq = NULL;
static AppTimer *s_timer = NULL;

static int s_index = 0;
static bool s_loop = false;

static void next_frame_handler(void *context) {
  // Draw the next frame
  layer_mark_dirty(s_canvas_layer);

  // Continue the sequence
  s_timer = app_timer_register(DELTA, next_frame_handler, NULL);
}

static void update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GSize seq_bounds = gdraw_command_sequence_get_bounds_size(s_command_seq);

  // Get the next frame
  GDrawCommandFrame *frame = gdraw_command_sequence_get_frame_by_index(s_command_seq, s_index);

  // If another frame was found, draw it
  if (frame) {
    gdraw_command_frame_draw(ctx, s_command_seq, frame, GPoint(
      (bounds.size.w - seq_bounds.w) / 2,
      (bounds.size.h - seq_bounds.h) / 2
    ));
  }

  // Advance to the next frame, wrapping if neccessary
  int num_frames = gdraw_command_sequence_get_num_frames(s_command_seq);
  s_index++;
  if (s_index == num_frames) {
    if(s_loop){
      s_index = 0;
    }
    else {
      if(s_timer){
        app_timer_cancel(s_timer);
        s_timer = NULL;
      }
      window_stack_remove(s_main_window, true);
    }
  }
}

static void main_window_load(Window *window) {
  // Get Window information
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the canvas Layer
  s_canvas_layer = layer_create(bounds);

  // Set the LayerUpdateProc
  layer_set_update_proc(s_canvas_layer, update_proc);

  // Add to parent Window
  layer_add_child(window_layer, s_canvas_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
  s_canvas_layer = NULL;
  gdraw_command_sequence_destroy(s_command_seq);
  s_command_seq = NULL;
  window_destroy(s_main_window);
  s_main_window = NULL;
  if(s_timer){
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
}

static void main_window_disappear(Window *window) {
  window_stack_remove(s_main_window, true);
}

void show_pdc_window(uint32_t resource, bool loop){
  if(s_command_seq){
    gdraw_command_sequence_destroy(s_command_seq);
  }

  // Load the first sequence
  s_command_seq = gdraw_command_sequence_create_with_resource(resource);
  s_loop = loop;
  s_index = 0;

  if(!s_main_window){
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = main_window_load,
      .unload = main_window_unload,
      .disappear = main_window_disappear,
    });
    window_stack_push(s_main_window, true);
  }

  // Start the animation
  if(s_timer){
    app_timer_cancel(s_timer);
  }
  s_timer = app_timer_register(DELTA, next_frame_handler, NULL);

 
}
