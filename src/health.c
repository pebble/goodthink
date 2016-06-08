#include <pebble.h>

#include "model.h"

static int32_t get_health_day_sum(HealthMetric metric){
  time_t start = time_start_of_today();
  time_t end = time(NULL);
  
  // Check the metric has data available for today
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
  
  if(mask & HealthServiceAccessibilityMaskAvailable) {
    return health_service_sum(metric, start, end);
  } else {
    return 0;
  }
}

static uint32_t get_health_hour_data(HealthMinuteData *data){
  const uint32_t max_records = 60;

  time_t end = time(NULL);
  time_t start = end - SECONDS_PER_HOUR;
  
  return health_service_get_minute_history(data, max_records, &start, &end);
}

void get_health_day(DailySurvey *data){
  data->step_count = get_health_day_sum(HealthMetricStepCount);
  data->active_seconds = get_health_day_sum(HealthMetricActiveSeconds);
  data->walked_distance_meters = get_health_day_sum(HealthMetricWalkedDistanceMeters);
  data->sleep_seconds = get_health_day_sum(HealthMetricSleepSeconds);
  data->sleep_restful_seconds = get_health_day_sum(HealthMetricSleepRestfulSeconds);
  data->resting_kcalories = get_health_day_sum(HealthMetricRestingKCalories);
  data->active_kcalories = get_health_day_sum(HealthMetricActiveKCalories);
}

void get_health_hour(HourlySurvey *data){
  data->step_count = get_health_day_sum(HealthMetricStepCount);
  data->active_seconds = get_health_day_sum(HealthMetricActiveSeconds);
  data->walked_distance_meters = get_health_day_sum(HealthMetricWalkedDistanceMeters);
  data->sleep_seconds = get_health_day_sum(HealthMetricSleepSeconds);
  data->sleep_restful_seconds = get_health_day_sum(HealthMetricSleepRestfulSeconds);
  data->resting_kcalories = get_health_day_sum(HealthMetricRestingKCalories);
  data->active_kcalories = get_health_day_sum(HealthMetricActiveKCalories);

  data->minute_data_num_records = get_health_hour_data(data->minute_data);
}
