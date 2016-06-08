#pragma once

#include "model.h"

void start_hourly_survey();

void start_daily_survey();

void resend_hourly_survey();

void resend_daily_survey();

const HourlySurvey* getHourlySurvey();

const DailySurvey* getDailySurvey();