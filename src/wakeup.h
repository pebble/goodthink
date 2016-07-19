#pragma once

#define EVENT_HOURLY 0
#define EVENT_DAILY  1

void schedule_event(uint8_t type_, bool force_);

void snooze_event(uint8_t type_);

void cancel_events();