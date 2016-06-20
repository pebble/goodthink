#include <pebble.h>

#include "survey_manager.h"
#include "enamel.h"
#include "persist.h"
#include "ui/pdc_window.h"
#include "ui/dialog_choice_window.h"
#include "vibrate.h"
#include "communication.h"

#define PKEY_EVENT_TYPE 0
#define EVENT_HOURLY 0
#define EVENT_DAILY  1

#define PKEY_START_TIME 1

#define MESSAGE_SNOOZE_HOURLY 0
#define MESSAGE_SNOOZE_DAILY  1
#define MESSAGE_CONF_CHANGED  2

#define KEY_SEND_RESPONSE 10

#define SURVEY1_PERSIST_KEY  100000
#define SURVEY2_PERSIST_KEY  110000
#define SURVEY3_PERSIST_KEY  120000
#define SURVEY4_PERSIST_KEY  130000

static uint32_t oldKey = 0;

static int s_event_type = EVENT_HOURLY;

static bool s_optin = false;

static void optin(bool enable);

static void sendOldData(){
  oldKey = 0;
  if(persist_exists(SURVEY1_PERSIST_KEY)){
    oldKey = SURVEY1_PERSIST_KEY;
  }
  else if(persist_exists(SURVEY2_PERSIST_KEY)){
    oldKey = SURVEY2_PERSIST_KEY;
  }
  else if(persist_exists(SURVEY3_PERSIST_KEY)){
    oldKey = SURVEY3_PERSIST_KEY;
  }
  else if(persist_exists(SURVEY4_PERSIST_KEY)){
    oldKey = SURVEY4_PERSIST_KEY;
  }
  
  if(oldKey){
    uint8_t* buffer = NULL;
    if(oldKey == SURVEY4_PERSIST_KEY){
      buffer = malloc(sizeof(DailySurvey));
      load_generic_data(oldKey, buffer, sizeof(DailySurvey));
      send_daily_survey((DailySurvey*)buffer, true);
    }
    else {
      buffer = malloc(sizeof(HourlySurvey));
      load_generic_data(oldKey, buffer, sizeof(HourlySurvey));
      send_hourly_survey((HourlySurvey*)buffer, true);
    }
    free(buffer);
  }
}

static void dismiss(){
  window_stack_pop_all(true);
  show_pdc_window(RESOURCE_ID_DISMISS_SEQUENCE, false);
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
    show_pdc_window(RESOURCE_ID_CONFIRM_SEQUENCE, false);
  }
  else {
    dialog_choice_window_push("Not connected\nTry again?", RESOURCE_ID_IMAGE_WARNING, s_event_type == EVENT_DAILY ? resend_daily_survey : resend_hourly_survey, RESOURCE_ID_IMAGE_TICK, 0, 0, dismiss, RESOURCE_ID_IMAGE_CROSS);
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *tuple_response = dict_find(iter, KEY_SEND_RESPONSE);
  if(tuple_response){
    if(oldKey){
      persist_delete(oldKey);
      sendOldData();
    }
    else {
      if(!persist_exists(PKEY_START_TIME)){
        time_t now = time(0);
        persist_write_data(PKEY_START_TIME, &now, sizeof(time_t));
      }
      show_pdc_window(RESOURCE_ID_CONFIRM_SEQUENCE, false);
    }
  }
  else {
    enamel_save();
    AppWorkerMessage message;
    app_worker_send_message(MESSAGE_CONF_CHANGED, &message);
    if(s_optin != enamel_get_optin()){
      optin(enamel_get_optin());
    }
  }
}

static void snooze(){
  AppWorkerMessage message;
  app_worker_send_message(s_event_type == EVENT_HOURLY ? MESSAGE_SNOOZE_HOURLY : MESSAGE_SNOOZE_DAILY, &message);
  window_stack_pop_all(true);
}

static void launch(){
  stop_vibe_timer();
  if(s_event_type == EVENT_HOURLY){
    start_hourly_survey();
  }
  else {
    start_daily_survey();
  }
  sendOldData();
}

static void exit_app(){
  window_stack_pop_all(true);
}

static void appMessageOutboxFailed(DictionaryIterator *iterator, AppMessageResult reason, void *context){
  if(oldKey == 0){
    saveOldData();
  }
  oldKey = 0;
}

static void optin(bool optin){
  s_optin = optin;
  window_stack_pop_all(false);
  if(!optin){
    dialog_choice_window_push(
        PBL_IF_ROUND_ELSE("You must opt-in to this app from the 'Settings' page on your phone.","\nYou must opt-in to this app\nfrom the 'Settings' page\non your phone."), 
        RESOURCE_ID_IMAGE_HAPPY,  
        0, 0,
        0, 0,  
        exit_app, RESOURCE_ID_IMAGE_CROSS);
    app_worker_kill();
  }
  else {
    if(persist_exists(PKEY_START_TIME)) {
      time_t now = time(0);
      time_t start_time;
      persist_read_data(PKEY_START_TIME,&start_time,sizeof(time_t));
      if(now > start_time + 7 * SECONDS_PER_DAY){
        dialog_choice_window_push(
          PBL_IF_ROUND_ELSE("Thank you for your participation.\n\nStay tuned for\nyour results.","Thank you for your participation.\n\nStay tuned for your results."), 
          RESOURCE_ID_IMAGE_HAPPY, 
          0, 0, exit_app, RESOURCE_ID_IMAGE_TICK, 0, 0);
        app_worker_kill();
        return;
      }
      else {
        AppLaunchReason reason = launch_reason();
        if(reason == APP_LAUNCH_WORKER) {
          s_event_type = persist_read_int(PKEY_EVENT_TYPE);
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
      dialog_choice_window_push(
          PBL_IF_ROUND_ELSE("Welcome to the Happiness App\nLet's get started with your first\ncheck-in!","Welcome to the Happiness App\n\nLet's get started with your first\ncheck-in!"), 
          RESOURCE_ID_IMAGE_HAPPY,  
          launch, RESOURCE_ID_IMAGE_TICK,
          0, 0,  
          dismiss, RESOURCE_ID_IMAGE_CROSS);
    }
    if(!app_worker_is_running()){
      AppWorkerResult result = app_worker_launch();
    }
  }

  
}

static void init(){
  enamel_init(0, 3000);
  enamel_register_custom_inbox_received(in_received_handler);
  app_message_register_outbox_failed(appMessageOutboxFailed);

  optin(enamel_get_optin());
}

static void deinit(){
	enamel_deinit();
  app_message_deregister_callbacks();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
