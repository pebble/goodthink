/**
 * This file was generated with Enamel : http://gregoiresage.github.io/enamel
 */

#ifndef enamel_h
#define enamel_h

#include <pebble.h>

// -----------------------------------------------------
// Getter for 'hourly_survey_start_time'
int32_t enamel_get_hourly_survey_start_time();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'hourly_survey_end_time'
int32_t enamel_get_hourly_survey_end_time();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'daily_survey_time'
int32_t enamel_get_daily_survey_time();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'snooze_option'
#define SNOOZE_OPTION_PRECISION 1
int32_t enamel_get_snooze_option();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'optin'
bool enamel_get_optin();
// -----------------------------------------------------

void enamel_register_custom_inbox_received(AppMessageInboxReceived received_callback); 

void enamel_init(const uint32_t size_inbound, const uint32_t size_outbound);

void enamel_deinit();

void enamel_save();

void enamel_load();

#endif