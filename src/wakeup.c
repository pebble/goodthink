#include <pebble.h>
#include "enamel.h"
#include "wakeup.h"

#define WKUP_ID_DAILY 	43
#define WKUP_ID_HOURLY 	44

static uint32_t prv_next_daily_event_timeout(){
	time_t t = time(NULL);
	time_t t_event = time_start_of_today() + enamel_get_daily_survey_time();

	if(t_event < t) {
		t_event += SECONDS_PER_DAY;
	}
	return t_event;
}

static uint32_t prv_next_hourly_event_timeout(){
	time_t t = time(NULL);
	time_t t_sod   = time_start_of_today();
	time_t t_event = t_sod;

	if(enamel_get_hourly_popups() == HOURLY_POPUPS_HOURLY){
		time_t t_start = t_sod + enamel_get_hourly_survey_start_time();
		time_t t_end   = t_sod + enamel_get_hourly_survey_end_time() ;
	
		if(t_end <= t_start){
			t_end += SECONDS_PER_DAY;
		}
	
		while(t_event < t_start){
			t_event += SECONDS_PER_HOUR;
		}
		while(t_event < t + SECONDS_PER_HOUR && t_event <= t_end) {
			t_event += SECONDS_PER_HOUR;
		}
		if(t_end < t_event){
			t_event = t_start + SECONDS_PER_DAY;
		}
	}
	else {
		if(t_sod + 20 * SECONDS_PER_HOUR < t){
			t_event += SECONDS_PER_DAY + 9 * SECONDS_PER_HOUR;
		}
		else if(t_sod + 17 * SECONDS_PER_HOUR < t){
			t_event += 20 * SECONDS_PER_HOUR;
		}
		else if(t_sod + 13 * SECONDS_PER_HOUR < t){
			t_event += 17 * SECONDS_PER_HOUR;
		}
		else if(t_sod + 9 * SECONDS_PER_HOUR < t){
			t_event += 13 * SECONDS_PER_HOUR;
		}
		else {
			t_event += 9 * SECONDS_PER_HOUR;
		}
	}
	return t_event;
}

static void prv_cancel_event(uint8_t type_){
	uint32_t key = type_ == EVENT_HOURLY ? WKUP_ID_HOURLY : WKUP_ID_DAILY;
	if(persist_exists(key)){
		WakeupId wakeup_id = persist_read_int(key);
		time_t wakeup_timestamp = 0;
		if(wakeup_query(wakeup_id, &wakeup_timestamp)){
			wakeup_cancel(wakeup_id);
		}
	}
}

static uint32_t prv_event_timestamp(uint8_t type_) {
	uint32_t key = type_ == EVENT_HOURLY ? WKUP_ID_HOURLY : WKUP_ID_DAILY;
	if(persist_exists(key)){
		WakeupId wakeup_id = persist_read_int(key);
		time_t wakeup_timestamp = 0;
		if(wakeup_query(wakeup_id, &wakeup_timestamp)){
			return wakeup_timestamp;
		}
	}
	return -1;
}

void schedule_event(uint8_t type_, bool force_){
	WakeupId id = -1;
	uint8_t try = 0;
	uint32_t next_time = type_ == EVENT_HOURLY ? prv_next_hourly_event_timeout() : prv_next_daily_event_timeout();
	force_ = force_ || next_time < prv_event_timestamp(type_);
	if(force_){
		prv_cancel_event(type_);
		while(id < 0 && try < 15){
			id = wakeup_schedule(next_time + SECONDS_PER_MINUTE * try, type_, true);
			try++;
		}
		if(id >= 0){
			persist_write_int(type_ == EVENT_HOURLY ? WKUP_ID_HOURLY : WKUP_ID_DAILY, id);
		}
	}
}

void snooze_event(uint8_t type_){
	prv_cancel_event(type_);
	WakeupId id = -1;
	uint8_t try = 0;
	uint32_t next_time = time(NULL) + enamel_get_snooze_option() * SECONDS_PER_MINUTE;
	while(id < 0 && try < 15){
		id = wakeup_schedule(next_time + SECONDS_PER_MINUTE * try, type_, true);
		try++;
	}
	if(id >= 0){
		persist_write_int(type_ == EVENT_HOURLY ? WKUP_ID_HOURLY : WKUP_ID_DAILY, id);
	}
}

void cancel_events(){
	wakeup_cancel_all();
}