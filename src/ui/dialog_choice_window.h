#pragma once

#include <pebble.h>
#include "model.h"

typedef void (*ActionHandler)(void);

void dialog_choice_window_push(const char* message, uint32_t image_id, ActionHandler up_handler, uint32_t up_image_id, ActionHandler select_handler, uint32_t select_image_id, ActionHandler down_handler, uint32_t down_image_id);