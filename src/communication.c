#include <pebble.h>

#include "model.h"

void send_hourly_survey(HourlySurvey* data, bool isold){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if(isold)
    dict_write_uint8(iter, MESSAGE_KEY_is_old, 1);
  dict_write_uint32(iter, MESSAGE_KEY_user_time, data->time_sec);
  dict_write_uint8(iter, MESSAGE_KEY_event_type, EVENT_TYPE_HOURLY);
  dict_write_uint8(iter, MESSAGE_KEY_energy_score, data->energy_level);
  dict_write_uint8(iter, MESSAGE_KEY_mood_id, data->mood_level);
  dict_write_uint8(iter, MESSAGE_KEY_location_id, data->location_id);
  dict_write_data(iter, MESSAGE_KEY_activity_ids, data->activity_ids, data->activities_count);
  dict_write_uint8(iter, MESSAGE_KEY_people_id, data->people_id);
  if(data->people_other[0])
    dict_write_cstring(iter, MESSAGE_KEY_people_other, data->people_other);
  if(data->activity_other[0])
    dict_write_cstring(iter, MESSAGE_KEY_activity_other, data->activity_other);
  if(data->location_other[0])
    dict_write_cstring(iter, MESSAGE_KEY_location_other, data->location_other);
  dict_write_int32(iter, MESSAGE_KEY_health_stepcount,            data->step_count);
  dict_write_int32(iter, MESSAGE_KEY_health_activeseconds,        data->active_seconds);
  dict_write_int32(iter, MESSAGE_KEY_health_walkeddistancemeters, data->walked_distance_meters);
  dict_write_int32(iter, MESSAGE_KEY_health_sleepseconds,         data->sleep_seconds);
  dict_write_int32(iter, MESSAGE_KEY_health_sleeprestfulseconds,  data->sleep_restful_seconds);
  dict_write_int32(iter, MESSAGE_KEY_health_restingkcalories,     data->resting_kcalories);
  dict_write_int32(iter, MESSAGE_KEY_health_activekcalories,      data->active_kcalories);

  uint32_t num_records = data->minute_data_num_records;
  uint8_t* records = malloc(num_records * sizeof(uint8_t));
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].steps;
  }
  dict_write_data(iter, MESSAGE_KEY_health_minute_steps, records, num_records);
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].orientation;
  }
  dict_write_data(iter, MESSAGE_KEY_health_minute_orientation, records, num_records);
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].vmc;
  }
  dict_write_data(iter, MESSAGE_KEY_health_minute_vmc, records, num_records);
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].is_invalid;
  }
  dict_write_data(iter, MESSAGE_KEY_health_minute_is_invalid, records, num_records);
  free(records);
  dict_write_end(iter);
  app_message_outbox_send();
}

void send_daily_survey(DailySurvey* data, bool isold){
  DictionaryIterator *iter;
  uint8_t value = 1;
  app_message_outbox_begin(&iter);
  if(isold)
    dict_write_uint8(iter, MESSAGE_KEY_is_old, 1);
  dict_write_uint32(iter, MESSAGE_KEY_user_time, data->time_sec);
  dict_write_uint8(iter, MESSAGE_KEY_event_type, EVENT_TYPE_DAILY);
  dict_write_uint8(iter, MESSAGE_KEY_satisfaction_level, data->satisfaction_level);
  dict_write_uint8(iter, MESSAGE_KEY_feeling_id, data->feeling_id);
  if(data->feeling_more && data->feeling_more[0])
    dict_write_cstring(iter, MESSAGE_KEY_feeling_day_more, data->feeling_more);
  dict_write_int32(iter, MESSAGE_KEY_health_stepcount,            data->step_count);
  dict_write_int32(iter, MESSAGE_KEY_health_activeseconds,        data->active_seconds);
  dict_write_int32(iter, MESSAGE_KEY_health_walkeddistancemeters, data->walked_distance_meters);
  dict_write_int32(iter, MESSAGE_KEY_health_sleepseconds,         data->sleep_seconds);
  dict_write_int32(iter, MESSAGE_KEY_health_sleeprestfulseconds,  data->sleep_restful_seconds);
  dict_write_int32(iter, MESSAGE_KEY_health_restingkcalories,     data->resting_kcalories);
  dict_write_int32(iter, MESSAGE_KEY_health_activekcalories,      data->active_kcalories);
  dict_write_end(iter);
  app_message_outbox_send();
}

void send_start_survey(uint32_t time_){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint32(iter, MESSAGE_KEY_user_time, time_);
  dict_write_uint8(iter, MESSAGE_KEY_event_type, EVENT_TYPE_START);
  dict_write_end(iter);
  app_message_outbox_send();
}

void send_stop_survey(uint32_t time_){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint32(iter, MESSAGE_KEY_user_time, time_);
  dict_write_uint8(iter, MESSAGE_KEY_event_type, EVENT_TYPE_END);
  dict_write_end(iter);
  app_message_outbox_send();
}