/*
 * weatherData.cpp: File which contains the definitioons of the functions used for 
 * getting, processing and displaying the weather data forecast and the current weather data
 */

#include "weatherData.h"

String getWeatherDataForecastRaw()
{
  WiFiClient client;
  char connectionAddress[] = "www.7timer.info";
  int connectionPort = 80;
  int DELAYWAITINGRESPONSE = 1000;
  int DELAYWAITINGNEWLINE = 100;

  Serial.print("Getting the weather data forecast... ");
  
  if (client.connect(connectionAddress, connectionPort))
  {
    // Connected
    // Send a HTTP GET request:  
    client.println("GET /bin/civillight.php?lon=14.438&lat=50.076&ac=0&unit=metric&output=json&tzshift=0 HTTP/1.1");
    client.println("Host: www.7timer.info");
    client.println("Connection: close");
    client.println();
  }
  else
  {
    // Not connected
  } 

  delay(DELAYWAITINGRESPONSE);

  // Get the HTTP GET response
  String currentResponseLine = "";
  String weatherDataForecastRaw = "";
  bool flagWeatherDataForecastRaw = false;
  
  while (client.connected()) 
  {
    currentResponseLine = client.readStringUntil('\n');

    delay(DELAYWAITINGNEWLINE);

    if (currentResponseLine == "{")
    {
      flagWeatherDataForecastRaw = true;
    }

    if (flagWeatherDataForecastRaw == true)
    {
      weatherDataForecastRaw = weatherDataForecastRaw + currentResponseLine;
    }

    if (currentResponseLine == "}") 
    {
      break;
    }
  }

  Serial.println("Done");

  return weatherDataForecastRaw;
}

void parseWeatherDataForecastRaw(String weatherDataForecastRaw, weatherDataForecast& wdf)
{
  JSONVar weatherDataForecastParsed = JSON.parse(weatherDataForecastRaw);

  for (int day = 0; day < 7; day++)
  {
    wdf.date[day] = JSON.stringify(weatherDataForecastParsed["dataseries"][day]["date"]);
    wdf.weather[day] = JSON.stringify(weatherDataForecastParsed["dataseries"][day]["weather"]);
    wdf.tempMax[day] = weatherDataForecastParsed["dataseries"][day]["temp2m"]["max"];
    wdf.tempMin[day] = weatherDataForecastParsed["dataseries"][day]["temp2m"]["min"];
    wdf.windSpeed[day] = weatherDataForecastParsed["dataseries"][day]["wind10m_max"];
  }
}

void displayWeatherDataForecast(MKRIoTCarrier carrier, weatherDataForecast wdf, int dayIndex)
{
    // Get the day, month and year as integers
    int day, month, year;
    transformDateFromString2Ints(wdf.date[dayIndex], day, month, year);

    // Get the weekday as integer and then as string
    int weekdayInt = calculateWeekday(day, month, year);
    String weekdayStr;
    switch (weekdayInt)
    {
      case 0: weekdayStr = "Su"; break;
      case 1: weekdayStr = "Mo"; break;
      case 2: weekdayStr = "Tu"; break;
      case 3: weekdayStr = "We"; break;
      case 4: weekdayStr = "Th"; break;
      case 5: weekdayStr = "Fr"; break;
      case 6: weekdayStr = "Sa"; break;
    }
  
    carrier.display.fillScreen(ST77XX_BLUE);
    carrier.display.setTextColor(ST77XX_WHITE);
    
    carrier.display.setTextSize(3);

    // Display the weekday and the date
    carrier.display.setCursor(50, 40);
    carrier.display.print(weekdayStr);
    carrier.display.print(" ");
    if (day < 10)
    {
      carrier.display.print("0");
    }
    carrier.display.print(day);
    carrier.display.print(".");
    if (month < 10)
    {
      carrier.display.print("0");
    }
    carrier.display.print(month);

    carrier.display.setTextSize(2);

    // Display the weather type
    String weatherType;

    if (wdf.weather[dayIndex] == "\"clear\"") weatherType = "Clear";
    else if (wdf.weather[dayIndex] == "\"pcloudy\"") weatherType = "Partly Cloudy";
    else if (wdf.weather[dayIndex] == "\"mcloudy\"") weatherType = "Mostly Cloudy";
    else if (wdf.weather[dayIndex] == "\"cloudy\"") weatherType = "Cloudy";
    else if (wdf.weather[dayIndex] == "\"humid\"") weatherType = "Humid";
    else if (wdf.weather[dayIndex] == "\"lightrain\"") weatherType = "Light Rain";
    else if (wdf.weather[dayIndex] == "\"oshower\"") weatherType = "Occasional showers";
    else if (wdf.weather[dayIndex] == "\"ishower\"") weatherType = "Isolated showers";
    else if (wdf.weather[dayIndex] == "\"rain\"") weatherType = "Rain";
    else if (wdf.weather[dayIndex] == "\"lightsnow\"") weatherType = "Light Snow";
    else if (wdf.weather[dayIndex] == "\"snow\"") weatherType = "Snow";
    else if (wdf.weather[dayIndex] == "\"rainsnow\"") weatherType = "Freezing Rain";
    else weatherType = wdf.weather[dayIndex];
    
    carrier.display.setCursor(20, 85);
    carrier.display.print(weatherType);

    // Display the minimum temperature
    carrier.display.setCursor(20, 110);
    carrier.display.print("Temp Min: ");
    carrier.display.print(wdf.tempMin[dayIndex]);
    carrier.display.print(" \370C");

    // Display the maximum temperature
    carrier.display.setCursor(20, 135);
    carrier.display.print("Temp Max: ");
    carrier.display.print(wdf.tempMax[dayIndex]);
    carrier.display.print(" \370C");

    // Display the wind type
    String windType;
    switch(wdf.windSpeed[dayIndex])
    {
      case 1: windType = "calm"; break;
      case 2: windType = "light"; break;
      case 3: windType = "moderate"; break;
      case 4: windType = "gresh"; break;
      case 5: windType = "strong"; break;
      case 6: windType = "gale"; break;
      case 7: windType = "storm"; break;
      case 8: windType = "hurricane"; break;
    }

    carrier.display.setCursor(20, 160);
    carrier.display.print("Wind: ");
    carrier.display.print(windType);
    
    delay(500);
}

weatherDataCurrent getWeatherDataCurrent(MKRIoTCarrier carrier)
{
  weatherDataCurrent wdc;

  wdc.temperature = carrier.Env.readTemperature();
  wdc.pressure = carrier.Pressure.readPressure();
  wdc.humidity = carrier.Env.readHumidity();

  return wdc;
}

void displayWeatherDataCurrent(MKRIoTCarrier carrier, weatherDataCurrent wdc)
{
  carrier.display.fillScreen(ST77XX_GREEN);
  carrier.display.setTextColor(ST77XX_BLACK);

  carrier.display.setTextSize(3);

  // Display the weekday and the date
  carrier.display.setCursor(90, 40);
  carrier.display.print("Now");
    
  carrier.display.setTextSize(2);

  // Display the temperature
  carrier.display.setCursor(20, 85);
  carrier.display.print("Temp: ");
  carrier.display.print(wdc.temperature);
  carrier.display.print(" \370C");

  // Display the pressure
  carrier.display.setCursor(20, 110);
  carrier.display.print("Pres: ");
  carrier.display.print(wdc.pressure);
  carrier.display.print(" kPa");

  // Display the humidity
  carrier.display.setCursor(20, 135);
  carrier.display.print("Humid: ");
  carrier.display.print(wdc.humidity);
  carrier.display.print(" %");
}

void displayUpdatingMessage(MKRIoTCarrier carrier)
{
  carrier.display.fillScreen(ST77XX_RED);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(3);
  
  carrier.display.setCursor(50, 60);
  carrier.display.print("UPDATING");
  
  carrier.display.setCursor(20, 110);
  carrier.display.print("THE WEATHER");
  
  carrier.display.setCursor(80, 160);
  carrier.display.print("DATA");
}

int calculateWeekday(int day, int month, int year)
{
  // Based on the formula from: https://cs.uwaterloo.ca/~alopez-o/math-faq/node73.html
  
  int century = (year % 100) == 0 ? (year / 100) - 1 : year / 100; 
  year = (month <= 2) ? year % 100 - 1 : year % 100; 
  month = (month <= 2) ? month - 2 + 12 : month - 2;  
  
  int weekday =  day 
                 + (int)floor(2.6 * month - 0.2)
                 - 2 * century
                 + year 
                 + (int)floor(year / 4.0)
                 + (int)floor(century / 4.0);

  while (weekday < 0)
  {
    weekday += 7;
  }

  weekday %= 7;

  return weekday;
}

void transformDateFromString2Ints(String date, int& day, int& month, int& year)
{
  year = date.substring(0, 4).toInt();
  month = date.substring(4, 6).toInt();
  day = date.substring(6, 8).toInt();
}
