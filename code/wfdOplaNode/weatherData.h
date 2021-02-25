#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <Arduino.h>
#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_JSON.h>

typedef struct weatherDataForecast
{
  String date[7];
  String weather[7];
  int tempMax[7], tempMin[7], windSpeed[7];
} weatherData;

typedef struct weatherDataCurrent
{
  float temperature;
  float pressure;
  float humidity;
} localWeatherData;

String getWeatherDataForecastRaw();
void parseWeatherDataForecastRaw(String weatherDataRaw, weatherDataForecast& wdf);
void displayWeatherDataForecast(MKRIoTCarrier carrier, weatherDataForecast wdf, int dayIndex);

weatherDataCurrent getWeatherDataCurrent(MKRIoTCarrier carrier);
void displayWeatherDataCurrent(MKRIoTCarrier carrier, weatherDataCurrent wdc);

void displayUpdatingMessage(MKRIoTCarrier carrier);
int calculateWeekday(int day, int month, int year);
void transformDateFromString2Ints(String date, int& day, int& month, int& year);

#endif
