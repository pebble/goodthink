#include <pebble.h>

#include "model.h"
#include "constants.h"
#include "communication.h"
#include "health.h"

#include "ui/energy_window.h"
#include "ui/mood_window.h"
#include "ui/location_window.h"
#include "ui/people_window.h"
#include "ui/activity_window.h"
#include "ui/feeling_window.h"
#include "ui/satisfaction_window.h"
#include "ui/dialog_choice_window.h"
#include "ui/pdc_window.h"

static HourlySurvey s_hourly_survey;
static DailySurvey s_daily_survey;

void resend_hourly_survey(){
	show_pdc_window(RESOURCE_ID_SENT_SEQUENCE, true);
	send_hourly_survey(&s_hourly_survey, false);
}

void resend_daily_survey(){
	show_pdc_window(RESOURCE_ID_SENT_SEQUENCE, true);
	send_daily_survey(&s_daily_survey, false);
}

static void add_health_data_hour(){
	get_health_hour(&s_hourly_survey);
	resend_hourly_survey();
}

static void activity_changed(bool* selection, char* other){
	s_hourly_survey.activities_count = 0;
	memcpy(s_hourly_survey.activity_other, other, MAX_TEXT_LENGTH);
    for(uint8_t i = 0; i < NB_OF_ACTIVITIES; i++) {
    	if(selection[i]){
    		s_hourly_survey.activity_ids[s_hourly_survey.activities_count] = ACTIVITY_IDS[i];
    		s_hourly_survey.activities_count++;
      		APP_LOG(APP_LOG_LEVEL_INFO, "Activity %s was selected", ACTIVITY_TEXTS[i]);
    	}
    }
    window_stack_pop_all(true);
    add_health_data_hour();
}

static void people_changed(uint8_t selection, char* other){
	s_hourly_survey.people_id = PEOPLE_IDS[selection];
	memcpy(s_hourly_survey.people_other, other, MAX_TEXT_LENGTH);
    APP_LOG(APP_LOG_LEVEL_INFO, "People %s was selected", PEOPLE_TEXTS[selection]);
	show_activity_window(activity_changed);
}

static void location_changed(uint8_t selection, char* other){
	s_hourly_survey.location_id = LOCATION_IDS[selection];
	memcpy(s_hourly_survey.location_other, other, MAX_TEXT_LENGTH);
	APP_LOG(APP_LOG_LEVEL_INFO, "Location : %d", selection);
	show_people_window(people_changed);
}

static void mood_changed(MoodLevel level){
	s_hourly_survey.mood_level = level;
	APP_LOG(APP_LOG_LEVEL_INFO, "MoodLevel : %d", level);
	show_location_window(location_changed);
}

static void energy_level_changed(EnergyLevel level){
	s_hourly_survey.energy_level = level;
	APP_LOG(APP_LOG_LEVEL_INFO, "EnergyLevel : %d", level);
	show_mood_window(50, mood_changed);
}

void start_hourly_survey(){
	memset(&s_hourly_survey, 0, sizeof(HourlySurvey));
	s_hourly_survey.time_sec = time(0);
	show_energy_window(50, energy_level_changed);
}

static void add_health_data_day(){
	get_health_day(&s_daily_survey);
	resend_daily_survey();
}

static DictationSession *s_dictation_session = NULL;
static char s_feeling_more[MAX_TEXT_LENGTH];

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, char *transcription, void *context) {
	if(status == DictationSessionStatusSuccess) {
	  snprintf(s_feeling_more, sizeof(s_feeling_more), "%s", transcription);
	} 
	dictation_session_destroy(s_dictation_session);
	memcpy(s_daily_survey.feeling_more, s_feeling_more, MAX_TEXT_LENGTH);
	add_health_data_day();
}

static void feeling_more(){
	s_feeling_more[0] = 0;
	s_dictation_session = dictation_session_create(sizeof(s_feeling_more), dictation_session_callback, NULL);
    dictation_session_start(s_dictation_session);
}

static void satisfaction_level_changed(SatisfactionLevel level){
	s_daily_survey.satisfaction_level = level;
	APP_LOG(APP_LOG_LEVEL_INFO, "SatisfactionLevel : %d", level);
	window_stack_pop_all(true);
	dialog_choice_window_push("Tell us more about your feelings on today", 0, feeling_more, RESOURCE_ID_IMAGE_TICK, 0, 0, add_health_data_day, RESOURCE_ID_IMAGE_CROSS);
}

static void feeling_changed(uint8_t selection){
	s_daily_survey.feeling_id = FEELING_IDS[selection];
	APP_LOG(APP_LOG_LEVEL_INFO, "Feeling %s was selected", FEELING_TEXTS[selection]);
	show_satisfaction_window(50, satisfaction_level_changed);
}

void start_daily_survey(){
	memset(&s_daily_survey, 0, sizeof(DailySurvey));
	s_daily_survey.time_sec = time(0);
	show_feeling_window(feeling_changed);
}

const HourlySurvey* getHourlySurvey(){
	return &s_hourly_survey;
}

const DailySurvey* getDailySurvey(){
	return &s_daily_survey;
}