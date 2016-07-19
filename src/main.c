#include <pebble.h>

#include "survey_manager.h"
#include "enamel.h"
#include "persist.h"
#include "ui/pdc_window.h"
#include "ui/dialog_choice_window.h"
#include "vibrate.h"
#include "communication.h"
#include "wakeup.h"

#include <pebble-events/pebble-events.h>

#define PKEY_START_TIME 1

#define SURVEY1_PERSIST_KEY  100000
#define SURVEY2_PERSIST_KEY  110000
#define SURVEY3_PERSIST_KEY  120000
#define SURVEY4_PERSIST_KEY  130000

static uint32_t s_oldKey = 0;

static int32_t s_event_type = EVENT_HOURLY;

static EventHandle s_inbox_received_handle;
static EventHandle s_outbox_failed_handle;

static void launch();
static void snooze();
static void dismiss();
static void exit_app();

static void sendOldData(){
  s_oldKey = 0;
  if(persist_exists(SURVEY1_PERSIST_KEY)){
    s_oldKey = SURVEY1_PERSIST_KEY;
  }
  else if(persist_exists(SURVEY2_PERSIST_KEY)){
    s_oldKey = SURVEY2_PERSIST_KEY;
  }
  else if(persist_exists(SURVEY3_PERSIST_KEY)){
    s_oldKey = SURVEY3_PERSIST_KEY;
  }
  else if(persist_exists(SURVEY4_PERSIST_KEY)){
    s_oldKey = SURVEY4_PERSIST_KEY;
  }
  
  if(s_oldKey){
    uint8_t* buffer = NULL;
    if(s_oldKey == SURVEY4_PERSIST_KEY){
      buffer = malloc(sizeof(DailySurvey));
      load_generic_data(s_oldKey, buffer, sizeof(DailySurvey));
      send_daily_survey((DailySurvey*)buffer, true);
    }
    else {
      buffer = malloc(sizeof(HourlySurvey));
      load_generic_data(s_oldKey, buffer, sizeof(HourlySurvey));
      send_hourly_survey((HourlySurvey*)buffer, true);
    }
    free(buffer);
  }
}

static void saveOldData(){
  uint32_t key = 0;
  if(s_event_type == EVENT_HOURLY){
    if(!persist_exists(SURVEY1_PERSIST_KEY)){
      key = SURVEY1_PERSIST_KEY;
    }
    else if(!persist_exists(SURVEY2_PERSIST_KEY)){
      key = SURVEY2_PERSIST_KEY;
    }
    else if(!persist_exists(SURVEY3_PERSIST_KEY)){
      key = SURVEY3_PERSIST_KEY;
    }
  }
  else {
    if(!persist_exists(SURVEY4_PERSIST_KEY)){
      key = SURVEY4_PERSIST_KEY;
    }
  }

  if(key){
    save_generic_data(key, 
      s_event_type == EVENT_DAILY ? (const void *)getDailySurvey() : (const void *)getHourlySurvey(), 
      s_event_type == EVENT_DAILY ? sizeof(DailySurvey) : sizeof(HourlySurvey));
    schedule_event(s_event_type, false);
    schedule_event(EVENT_HOURLY, false);
    schedule_event(EVENT_DAILY,  false);
    show_pdc_window(RESOURCE_ID_CONFIRM_SEQUENCE, false);
  }
  else {
    dialog_choice_window_push("Not connected\nTry again?", RESOURCE_ID_IMAGE_WARNING, s_event_type == EVENT_DAILY ? resend_daily_survey : resend_hourly_survey, RESOURCE_ID_IMAGE_TICK, 0, 0, dismiss, RESOURCE_ID_IMAGE_CROSS);
  }
}

static void prv_inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple *tuple_response = dict_find(iter, MESSAGE_KEY_send_response);
  if(tuple_response){
    if(s_oldKey){
      persist_delete(s_oldKey);
      sendOldData();
    }
    else {
      if(!persist_exists(PKEY_START_TIME)){
        time_t now = time(0);
        persist_write_data(PKEY_START_TIME, &now, sizeof(time_t));
      }
      schedule_event(EVENT_HOURLY, false);
      schedule_event(EVENT_DAILY,  false);
      show_pdc_window(RESOURCE_ID_CONFIRM_SEQUENCE, false);
    }
  }
}

static void prv_outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context){
  if(s_oldKey == 0){
    saveOldData();
  }
  s_oldKey = 0;
}

static void prv_set_optin(bool optin){
  window_stack_pop_all(false);
  if(!optin){
    cancel_events();
    dialog_choice_window_push(
        PBL_IF_ROUND_ELSE("You must opt-in to this app from the 'Settings' page on your phone.","\nYou must opt-in to this app\nfrom the 'Settings' page\non your phone."), 
        RESOURCE_ID_IMAGE_HAPPY,  
        0, 0,
        0, 0,  
        exit_app, RESOURCE_ID_IMAGE_CROSS);
  }
  else {
    if(persist_exists(PKEY_START_TIME)) {
      time_t now = time(0);
      time_t start_time;
      persist_read_data(PKEY_START_TIME,&start_time,sizeof(time_t));
      APP_LOG(0, "%ld %d ", start_time, enamel_get_study_duration());
      if(now > start_time + enamel_get_study_duration() * SECONDS_PER_DAY){
        cancel_events();
        dialog_choice_window_push(
          PBL_IF_ROUND_ELSE("Thank you for your participation.\n\nStay tuned for\nyour results.","Thank you for your participation.\n\nStay tuned for your results."), 
          RESOURCE_ID_IMAGE_HAPPY, 
          0, 0, exit_app, RESOURCE_ID_IMAGE_TICK, 0, 0);
        return;
      }
      else {
        schedule_event(EVENT_HOURLY, false);
        schedule_event(EVENT_DAILY,  false);
        if(launch_reason() == APP_LAUNCH_WAKEUP) {
          WakeupId id = 0;
          wakeup_get_launch_event(&id, &s_event_type);
          launch_vibe_timer();
        }
        dialog_choice_window_push(
          s_event_type == EVENT_HOURLY ? HOURLY_SURVEY_MESSAGE : DAILY_SURVEY_MESSAGE, 
          s_event_type == EVENT_HOURLY ? RESOURCE_ID_IMAGE_MOOD : RESOURCE_ID_IMAGE_DAY, 
          dismiss, RESOURCE_ID_IMAGE_CROSS, 
          launch, RESOURCE_ID_IMAGE_TICK, 
          snooze, RESOURCE_ID_IMAGE_ZZZ);
      }
    }
    else 
    {
      cancel_events();
      dialog_choice_window_push(
          PBL_IF_ROUND_ELSE("Welcome to the Happiness App\nLet's get started with your first\ncheck-in!","Welcome to the Happiness App\n\nLet's get started with your first\ncheck-in!"), 
          RESOURCE_ID_IMAGE_HAPPY,  
          launch, RESOURCE_ID_IMAGE_TICK,
          0, 0,  
          dismiss, RESOURCE_ID_IMAGE_CROSS);
    }
  }
}

static void updateSettings() {
  if(enamel_get_optin()){
    schedule_event(EVENT_HOURLY, true);
    schedule_event(EVENT_DAILY,  true);
  }
  prv_set_optin(enamel_get_optin());
}

static void dismiss(){
  schedule_event(s_event_type, false);
  window_stack_pop_all(true);
  show_pdc_window(RESOURCE_ID_DISMISS_SEQUENCE, false);
}

static void snooze(){
  snooze_event(s_event_type);
  window_stack_pop_all(true);
}

static void launch(){
  stop_vibe_timer();
  s_event_type == EVENT_HOURLY ? start_hourly_survey() : start_daily_survey();
  sendOldData();
}

static void exit_app(){
  window_stack_pop_all(true);
}

static void init(){
  enamel_init();

  enamel_register_settings_received(updateSettings);

  s_inbox_received_handle = events_app_message_register_inbox_received(prv_inbox_received_callback, NULL);
  s_outbox_failed_handle = events_app_message_register_outbox_failed(prv_outbox_failed_callback, NULL);

  events_app_message_open(); 

  prv_set_optin(enamel_get_optin());
}

static void deinit(){
	enamel_deinit();
  events_app_message_unsubscribe(s_inbox_received_handle);
  events_app_message_unsubscribe(s_outbox_failed_handle);
  app_message_deregister_callbacks();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
