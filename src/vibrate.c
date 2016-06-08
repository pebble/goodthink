#include <pebble.h>

static AppTimer *vibe_timer;

void stop_vibe_timer(){
  if(vibe_timer){
    app_timer_cancel(vibe_timer);
    vibe_timer = NULL;
  }
}

static void vibrate(void *data){
  static const uint32_t const segments[] = { 500, 400, 500 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  vibes_enqueue_custom_pattern(pat);
  vibe_timer = app_timer_register(30 * 1000, vibrate, NULL);
}

void launch_vibe_timer(){
  if(vibe_timer){
    stop_vibe_timer();
  }
  vibe_timer = app_timer_register(100, vibrate, NULL);
}
