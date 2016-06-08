#pragma once

#include <pebble.h>
#include "constants.h"

typedef uint8_t EnergyLevel;
#define ENERGY_LEVEL_MAX 100
typedef void (*EnergyLevelChangedHandler)(EnergyLevel level);


typedef uint8_t MoodLevel;
#define MOOD_LEVEL_MAX 100
typedef void (*MoodChangedHandler)(MoodLevel level);

typedef void (*PeopleChangedHandler)(uint8_t selection, char* other);

typedef void (*ActivityChangedHandler)(bool* selection, char* other);

typedef void (*FeelingChangedHandler)(uint8_t selection);

typedef void (*LocationChangedHandler)(uint8_t selection, char* other);

typedef uint8_t SatisfactionLevel;
#define SATISFACTION_LEVEL_MAX 100
typedef void (*SatisfactionLevelChangedHandler)(SatisfactionLevel level);

typedef struct {
  uint32_t time_sec;

  EnergyLevel energy_level;
  MoodLevel mood_level;
  uint8_t location_id;
  char location_other[MAX_TEXT_LENGTH];
  uint8_t people_id;
  char people_other[MAX_TEXT_LENGTH];
  uint8_t activity_ids[NB_OF_ACTIVITIES];
  uint8_t activities_count;
  char activity_other[MAX_TEXT_LENGTH];

  uint32_t step_count;
  uint32_t active_seconds;
  uint32_t walked_distance_meters;
  uint32_t sleep_seconds;
  uint32_t sleep_restful_seconds;
  uint32_t resting_kcalories;
  uint32_t active_kcalories;

  uint8_t minute_data_num_records;
  HealthMinuteData minute_data[60];

} HourlySurvey;

typedef struct {
  uint32_t time_sec;
  
  SatisfactionLevel satisfaction_level;
  uint8_t feeling_id;
  char feeling_more[MAX_TEXT_LENGTH];

  uint32_t step_count;
  uint32_t active_seconds;
  uint32_t walked_distance_meters;
  uint32_t sleep_seconds;
  uint32_t sleep_restful_seconds;
  uint32_t resting_kcalories;
  uint32_t active_kcalories;

} DailySurvey;

