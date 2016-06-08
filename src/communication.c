#include <pebble.h>

#include "model.h"

#define KEY_EVENT_TYPE 	105

#define KEY_ENERGY_SCORE    1001
#define KEY_MOOD_LEVEL      1002
#define KEY_PEOPLE_ID       1003
#define KEY_ACTIVITIES_ID   1004
#define KEY_STEPS_ID        1005
#define KEY_LOCATION_ID     1006
#define KEY_PEOPLE_OTHER    1007
#define KEY_ACTIVITY_OTHER  1008
#define KEY_LOCATION_OTHER  1009
#define KEY_TIME            1010
#define KEY_ISOLD           1011

#define KEY_HEALTH_STEPCOUNT             1100
#define KEY_HEALTH_ACTIVESECONDS         1101
#define KEY_HEALTH_WALKEDDISTANCEMETERS  1102
#define KEY_HEALTH_SLEEPSECONDS          1103
#define KEY_HEALTH_SLEEPRESTFULSECONDS   1104
#define KEY_HEALTH_RESTINGKCALORIES      1105
#define KEY_HEALTH_ACTIVEKCALORIES       1106

#define KEY_HEALTH_MINUTE_STEPS          1107
#define KEY_HEALTH_MINUTE_ORIENTATION    1108
#define KEY_HEALTH_MINUTE_VMC            1109
#define KEY_HEALTH_MINUTE_INVALID        1110

#define KEY_SATISFACTION_LEVEL 	2001
#define KEY_FEELING_ID 			    2002
#define KEY_FEELING_DAY_MORE    2003

void send_hourly_survey(HourlySurvey* data, bool isold){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if(isold)
    dict_write_uint8(iter, KEY_ISOLD, 1);
  dict_write_uint32(iter, KEY_TIME, data->time_sec);
  dict_write_uint8(iter, KEY_EVENT_TYPE, 0);
  dict_write_uint8(iter, KEY_ENERGY_SCORE, data->energy_level);
  dict_write_uint8(iter, KEY_MOOD_LEVEL, data->mood_level);
  dict_write_uint8(iter, KEY_LOCATION_ID, data->location_id);
  dict_write_data(iter, KEY_ACTIVITIES_ID, data->activity_ids, data->activities_count);
  dict_write_uint8(iter, KEY_PEOPLE_ID, data->people_id);
  if(data->people_other[0])
    dict_write_cstring(iter, KEY_PEOPLE_OTHER, data->people_other);
  if(data->activity_other[0])
    dict_write_cstring(iter, KEY_ACTIVITY_OTHER, data->activity_other);
  if(data->location_other[0])
    dict_write_cstring(iter, KEY_LOCATION_OTHER, data->location_other);
  dict_write_int32(iter, KEY_HEALTH_STEPCOUNT,            data->step_count);
  dict_write_int32(iter, KEY_HEALTH_ACTIVESECONDS,        data->active_seconds);
  dict_write_int32(iter, KEY_HEALTH_WALKEDDISTANCEMETERS, data->walked_distance_meters);
  dict_write_int32(iter, KEY_HEALTH_SLEEPSECONDS,         data->sleep_seconds);
  dict_write_int32(iter, KEY_HEALTH_SLEEPRESTFULSECONDS,  data->sleep_restful_seconds);
  dict_write_int32(iter, KEY_HEALTH_RESTINGKCALORIES,     data->resting_kcalories);
  dict_write_int32(iter, KEY_HEALTH_ACTIVEKCALORIES,      data->active_kcalories);

  uint32_t num_records = data->minute_data_num_records;
  uint8_t* records = malloc(num_records * sizeof(uint8_t));
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].steps;
  }
  dict_write_data(iter, KEY_HEALTH_MINUTE_STEPS, records, num_records);
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].orientation;
  }
  dict_write_data(iter, KEY_HEALTH_MINUTE_ORIENTATION, records, num_records);
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].vmc;
  }
  dict_write_data(iter, KEY_HEALTH_MINUTE_VMC, records, num_records);
  for(uint32_t i = 0; i < num_records; i++) {
    records[i] = data->minute_data[i].is_invalid;
  }
  dict_write_data(iter, KEY_HEALTH_MINUTE_INVALID, records, num_records);
  free(records);
  dict_write_end(iter);
  app_message_outbox_send();
}

void send_daily_survey(DailySurvey* data, bool isold){
  DictionaryIterator *iter;
  uint8_t value = 1;
  app_message_outbox_begin(&iter);
  if(isold)
    dict_write_uint8(iter, KEY_ISOLD, 1);
  dict_write_uint32(iter, KEY_TIME, data->time_sec);
  dict_write_uint8(iter, KEY_EVENT_TYPE, 1);
  dict_write_uint8(iter, KEY_SATISFACTION_LEVEL, data->satisfaction_level);
  dict_write_uint8(iter, KEY_FEELING_ID, data->feeling_id);
  if(data->feeling_more && data->feeling_more[0])
    dict_write_cstring(iter, KEY_FEELING_DAY_MORE, data->feeling_more);
  dict_write_int32(iter, KEY_HEALTH_STEPCOUNT,            data->step_count);
  dict_write_int32(iter, KEY_HEALTH_ACTIVESECONDS,        data->active_seconds);
  dict_write_int32(iter, KEY_HEALTH_WALKEDDISTANCEMETERS, data->walked_distance_meters);
  dict_write_int32(iter, KEY_HEALTH_SLEEPSECONDS,         data->sleep_seconds);
  dict_write_int32(iter, KEY_HEALTH_SLEEPRESTFULSECONDS,  data->sleep_restful_seconds);
  dict_write_int32(iter, KEY_HEALTH_RESTINGKCALORIES,     data->resting_kcalories);
  dict_write_int32(iter, KEY_HEALTH_ACTIVEKCALORIES,      data->active_kcalories);
  dict_write_end(iter);
  app_message_outbox_send();
}