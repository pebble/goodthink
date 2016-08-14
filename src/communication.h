#pragma once

#include "model.h"

void send_hourly_survey(HourlySurvey* data, bool isold);

void send_daily_survey(DailySurvey* data, bool isold);

void send_start_survey(uint32_t time);

void send_stop_survey(uint32_t time);