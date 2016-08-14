#pragma once

#define NB_OF_PEOPLES 6
static const char* PEOPLE_TEXTS[NB_OF_PEOPLES] = {
  "Nobody",
  "Friends",
  "Colleagues",
  "Family",
  "Significant other",
  "Other",
};
static const uint8_t PEOPLE_IDS[NB_OF_PEOPLES] = {
  0,
  1,
  2,
  3,
  4,
  0xFF,
};

#define NB_OF_ACTIVITIES 12
static const char* ACTIVITY_TEXTS[NB_OF_ACTIVITIES] = {
  "Drank caffeine",
  "Ate food",
  "Commuted",
  "Exercised",
  "Meeting",
  "Socialized",
  "Worked",
  "Drank water",
  "Yoga",
  "Meditated",
  "Drank alcohol",
  "Other"
};
static const uint8_t ACTIVITY_IDS[NB_OF_ACTIVITIES] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  0xFF,
};

#define NB_OF_FEELINGS 10
static const char* FEELING_TEXTS[NB_OF_ACTIVITIES] = {
  "Joyous",
  "Excite",
  "Happy",
  "Content",
  "Optimistic",
  "Sad",
  "Anxious",
  "Angry",
  "Depressed",
  "Despondent",
};
static const uint8_t FEELING_IDS[NB_OF_ACTIVITIES] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
};

#define NB_OF_LOCATIONS 5
static const char* LOCATION_TEXTS[NB_OF_LOCATIONS] = {
  "Home",
  "Work",
  "Commuting",
  "Social Event",
  "Other",
};
static const uint8_t LOCATION_IDS[NB_OF_LOCATIONS] = {
  0,
  1,
  2,
  3,
  0xFF,
};

#define HOURLY_SURVEY_MESSAGE "Mood time"
#define DAILY_SURVEY_MESSAGE  "Start the daily survey?"

#define MAX_TEXT_LENGTH 255

#define EVENT_TYPE_HOURLY 0
#define EVENT_TYPE_DAILY  1
#define EVENT_TYPE_START  2
#define EVENT_TYPE_END    3