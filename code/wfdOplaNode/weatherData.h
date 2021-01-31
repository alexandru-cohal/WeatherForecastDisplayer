#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <Arduino.h>
#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_JSON.h>

typedef struct weatherData
{
  String date[7];
  String weather[7];
  int tempMax[7], tempMin[7], windSpeed[7];
} weatherData;

String getWeatherDataRaw();
void parseWeatherDataRaw(String weatherDataRaw, weatherData& wd);
void displayWeatherData(MKRIoTCarrier carrier, weatherData wd, int dayIndex);
void displayUpdatingMessage(MKRIoTCarrier carrier);
int calculateWeekday(int day, int month, int year);
void transformDateFromString2Ints(String date, int& day, int& month, int& year);

#endif
