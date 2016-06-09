#include <pebble_worker.h>

#include "enamel.h"

static bool s_is_sleeping;

static AppTimer *s_hourly_timer;
static AppTimer *s_daily_timer;

#define PKEY_EVENT_TYPE 0
#define EVENT_HOURLY 0
#define EVENT_DAILY  1

#define MESSAGE_SNOOZE_HOURLY 0
#define MESSAGE_SNOOZE_DAILY  1
#define MESSAGE_CONF_CHANGED  2

static uint32_t next_daily_event_timeout(){
	time_t t = time(NULL);
	time_t t_event = time_start_of_today() + enamel_get_daily_survey_time() * SECONDS_PER_MINUTE;

	if(t > t_event) {
		t_event += SECONDS_PER_DAY;
	}
	return (t_event - t) * 1000;
}

static uint32_t next_hourly_event_timeout(){
	time_t t = time(NULL);
	time_t t_event = t + SECONDS_PER_HOUR;

	time_t t_sod   = time_start_of_today();
	time_t t_start = t_sod + enamel_get_hourly_survey_start_time() * SECONDS_PER_MINUTE;
	time_t t_end   = t_sod + enamel_get_hourly_survey_end_time()   * SECONDS_PER_MINUTE;

	if(t_end < t_start){
		if(t_end < t_event && t_event < t_start){
			t_event = t_start;
		}
	}
	else if(t_event < t_start) {
		t_event = t_start;
	}
	else if(t_end < t_event) {
		t_event = t_start + SECONDS_PER_DAY;
	}
	return (t_event - t) * 1000;
}

static uint32_t next_snooze_event_timeout(){
	return enamel_get_snooze_option() * SECONDS_PER_MINUTE * 1000;
}

static void hourly_timer_cb(void* context){
	s_hourly_timer = NULL;
	persist_write_int(PKEY_EVENT_TYPE, EVENT_HOURLY);
	worker_launch_app();
	s_hourly_timer = app_timer_register(next_hourly_event_timeout(), hourly_timer_cb, NULL);
}

static void daily_timer_cb(void* context){
	s_daily_timer = NULL;
	persist_write_int(PKEY_EVENT_TYPE, EVENT_DAILY);
	worker_launch_app();
	s_daily_timer = app_timer_register(next_daily_event_timeout(), daily_timer_cb, NULL);
}

#if defined(PBL_HEALTH)
static void health_handler(HealthEventType event, void *context) {
	if(event == HealthEventSleepUpdate) {
		// Get an activities mask
		HealthActivityMask activities = health_service_peek_current_activities();
		bool is_sleeping = activities & HealthActivitySleep || activities & HealthActivityRestfulSleep;
		if(is_sleeping) {
		  APP_LOG(APP_LOG_LEVEL_INFO, "The user is sleeping.");
		} else {
		  APP_LOG(APP_LOG_LEVEL_INFO, "The user is not currently sleeping.");
		}
		if(s_is_sleeping != is_sleeping) {
			if(is_sleeping) {
				if(s_hourly_timer){
					app_timer_cancel(s_hourly_timer);
					s_hourly_timer = NULL;
				}
			}
			else {
				if(s_hourly_timer){
					app_timer_cancel(s_hourly_timer);
					s_hourly_timer = NULL;
				}
				s_hourly_timer = app_timer_register(next_hourly_event_timeout(), hourly_timer_cb, NULL);
			}
		}
	}
}
#endif

static void message_handler(uint16_t type, AppWorkerMessage *data){
  enamel_load();
  if(type == MESSAGE_SNOOZE_HOURLY){
  	if(s_hourly_timer)
  		app_timer_cancel(s_hourly_timer);
    s_hourly_timer = app_timer_register(next_snooze_event_timeout(), hourly_timer_cb, NULL);
  } 
  else if(type == MESSAGE_SNOOZE_DAILY){
  	if(s_daily_timer)
  		app_timer_cancel(s_daily_timer);
    s_daily_timer = app_timer_register(next_snooze_event_timeout(), daily_timer_cb, NULL);
  }
  else if(type == MESSAGE_CONF_CHANGED){
    app_timer_cancel(s_daily_timer);
    s_daily_timer = app_timer_register(next_daily_event_timeout(), daily_timer_cb, NULL);
    app_timer_cancel(s_hourly_timer);
    s_hourly_timer = app_timer_register(next_hourly_event_timeout(), hourly_timer_cb, NULL);
  }
}

static void init() {
  enamel_load();

  s_is_sleeping = true;

  s_daily_timer = app_timer_register(next_daily_event_timeout(), daily_timer_cb, NULL);
  s_hourly_timer = app_timer_register(next_hourly_event_timeout(), hourly_timer_cb, NULL);

  app_worker_message_subscribe(message_handler);

#if defined(PBL_HEALTH)
  // Attempt to subscribe 
  if(health_service_events_subscribe(health_handler, NULL)) {
  	health_handler(HealthEventSleepUpdate, NULL);
  }
  else {
  	APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
#else
  APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
#endif

}

static void deinit() {
#if defined(PBL_HEALTH)
  health_service_events_unsubscribe();
#endif
  app_worker_message_unsubscribe();
}

int main(void) {
  init();
  worker_event_loop();
  deinit();
}