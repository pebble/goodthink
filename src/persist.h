#pragma once

#include <pebble.h>

void save_generic_data(uint32_t startkey, const void *data, uint16_t size);
void load_generic_data(uint32_t startkey, void *data, uint16_t size);