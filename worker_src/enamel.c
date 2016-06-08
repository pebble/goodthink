/**
 * This file was generated with Enamel : http://gregoiresage.github.io/enamel
 */

#include "enamel.h"

#ifndef ENAMEL_MAX_STRING_LENGTH
#define ENAMEL_MAX_STRING_LENGTH 30
#endif

// -----------------------------------------------------
// Getter for 'hourly_survey_start_time'
#define HOURLY_SURVEY_START_TIME_PKEY 10000
static int32_t _hourly_survey_start_time;
int32_t enamel_get_hourly_survey_start_time(){
	return _hourly_survey_start_time;
}
static void set_hourly_survey_start_time(int32_t value){
	_hourly_survey_start_time = value;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'hourly_survey_end_time'
#define HOURLY_SURVEY_END_TIME_PKEY 10001
static int32_t _hourly_survey_end_time;
int32_t enamel_get_hourly_survey_end_time(){
	return _hourly_survey_end_time;
}
static void set_hourly_survey_end_time(int32_t value){
	_hourly_survey_end_time = value;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'daily_survey_time'
#define DAILY_SURVEY_TIME_PKEY 10002
static int32_t _daily_survey_time;
int32_t enamel_get_daily_survey_time(){
	return _daily_survey_time;
}
static void set_daily_survey_time(int32_t value){
	_daily_survey_time = value;
}
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'snooze_option'
#define SNOOZE_OPTION_PKEY 10003
static int32_t _snooze_option;
int32_t enamel_get_snooze_option(){
	return _snooze_option;
}
static void set_snooze_option(int32_t value){
	_snooze_option = value;
}
// -----------------------------------------------------

void enamel_save(){
	persist_read_int(HOURLY_SURVEY_START_TIME_PKEY) != _hourly_survey_start_time ? persist_write_int(HOURLY_SURVEY_START_TIME_PKEY, _hourly_survey_start_time) : false;
	persist_read_int(HOURLY_SURVEY_END_TIME_PKEY) != _hourly_survey_end_time ? persist_write_int(HOURLY_SURVEY_END_TIME_PKEY, _hourly_survey_end_time) : false;
	persist_read_int(DAILY_SURVEY_TIME_PKEY) != _daily_survey_time ? persist_write_int(DAILY_SURVEY_TIME_PKEY, _daily_survey_time) : false;
	persist_read_int(SNOOZE_OPTION_PKEY) != _snooze_option ? persist_write_int(SNOOZE_OPTION_PKEY, _snooze_option) : false;
}

void enamel_load(){
	if (persist_exists(HOURLY_SURVEY_START_TIME_PKEY)) {
		set_hourly_survey_start_time(persist_read_int(HOURLY_SURVEY_START_TIME_PKEY));
	}
	else {
		set_hourly_survey_start_time(8 * 60);
	}
	if (persist_exists(HOURLY_SURVEY_END_TIME_PKEY)) {
		set_hourly_survey_end_time(persist_read_int(HOURLY_SURVEY_END_TIME_PKEY));
	}
	else {
		set_hourly_survey_end_time(21 * 60);
	}
	if (persist_exists(DAILY_SURVEY_TIME_PKEY)) {
		set_daily_survey_time(persist_read_int(DAILY_SURVEY_TIME_PKEY));
	}
	else {
		set_daily_survey_time(20 * 60);
	}
	if (persist_exists(SNOOZE_OPTION_PKEY)) {
		set_snooze_option(persist_read_int(SNOOZE_OPTION_PKEY));
	}
	else {
		set_snooze_option(10);
	}
}