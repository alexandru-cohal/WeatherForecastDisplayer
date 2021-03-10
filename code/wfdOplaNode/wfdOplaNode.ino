/*
 * wfdOplaNode.ino: The main source code file of the WeatherDataDisplayer project. 
 * 
 * Weather forecast data for 7 days (minimum and maximum temperature, weather type and wind type) and 
 * current (local) weather data (temperature, pressure and humidity) are obtained from internet and 
 * from the embedded sensors and displayed on the screen.
 * By default, the forecast for the current day is displayed on the screen. The touch buttons are used 
 * to change the data displayed:
 *  - Button 0: shows the forecast for the previous day (if available)
 *  - Button 4: shows the forecast for the next day (if available)
 *  - Button 2: shows the current (local) weather data
 * 
 * This file contains the setup function (which initializes the Serial port, the WiFi module and the MKRIoTCarrier) 
 * and the loop functions (in which the periodic update of the weather data is triggered and the 
 * status of the buttons is read and some actions are triggered accordingly)
 * 
 * IMPORTANT: The WiFi credentials have to be set in the arduino_secrets.h file!
 */

#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>

#include "arduino_secrets.h"
#include "weatherData.h"

// The weather data updating interval
// 120000 ms = 2 min
// 3600000 ms = 60 min
#define UPDATETIMEINTERVAL 3600000 // milliseconds

// The inactivity time interval.
// If no button is touched for this interval, the weather of the current day will be displayed
// 5000 ms = 5 s
// 10000 ms = 10 s
#define INACTIVITYTIMEINTERVAL 10000 // milliseconds

MKRIoTCarrier carrier;

void setup() 
{
  // Initialize the Serial data transmission
  Serial.begin(9600);

  // Connect to the WiFi network
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);
    
    // Wait 1 seconds for connection
    delay(1000);
  }
  Serial.println("Connected to wifi");

  // Enable the low power mode of the WiFi module
  WiFi.lowPowerMode();

  //Initialize the MKR IoT Carrier and output any errors in the serial monitor
  CARRIER_CASE = true;
  carrier.begin();
  carrier.display.setRotation(0);
}

void loop() 
{ 
  // Get the weather data periodically
    static weatherDataForecast weatherDataForecastParsed;
    static weatherDataCurrent weatherDataCurrentRaw;
    static unsigned long lastGetCallTime = -UPDATETIMEINTERVAL;
    unsigned long currentTime = millis();
    static bool flagWeatherDataForecastDisplayed = false;
  
    if (abs(currentTime - lastGetCallTime) >= UPDATETIMEINTERVAL)
    {
      lastGetCallTime = currentTime;

      flagWeatherDataForecastDisplayed = false;
      displayUpdatingMessage(carrier);
      
      String weatherDataForecastRaw = getWeatherDataForecastRaw();
      parseWeatherDataForecastRaw(weatherDataForecastRaw, weatherDataForecastParsed);

      flagWeatherDataForecastDisplayed = true;
      displayWeatherDataForecast(carrier, weatherDataForecastParsed, 0);

      weatherDataCurrentRaw = getWeatherDataCurrent(carrier);
    }

  // Read the buttons' status and display the weather data for the selected day
    static int indexDayToDisplay = 0;
    static unsigned long lastButtonTouchTime = 0;
    
    carrier.Buttons.update();
  
    // Move to the previous day
    if (carrier.Button0.onTouchDown()) 
    {
      if (flagWeatherDataForecastDisplayed == false)
      {
        flagWeatherDataForecastDisplayed = true;
        displayWeatherDataForecast(carrier, weatherDataForecastParsed, indexDayToDisplay); 
      }
      else if (indexDayToDisplay > 0)
      {
        indexDayToDisplay--;
        flagWeatherDataForecastDisplayed = true;
        displayWeatherDataForecast(carrier, weatherDataForecastParsed, indexDayToDisplay);
      }

      lastButtonTouchTime = currentTime;
    }
  
    // Move to the next day
    if (carrier.Button4.onTouchDown()) 
    {
      if (flagWeatherDataForecastDisplayed == false)
      {
        flagWeatherDataForecastDisplayed = true;
        displayWeatherDataForecast(carrier, weatherDataForecastParsed, indexDayToDisplay); 
      }
      else if (indexDayToDisplay < 6)
      {
        indexDayToDisplay++;
        flagWeatherDataForecastDisplayed = true;
        displayWeatherDataForecast(carrier, weatherDataForecastParsed, indexDayToDisplay);
      }

      lastButtonTouchTime = currentTime;
    }

    // Switch to the current weather data
    if (carrier.Button2.onTouchDown())
    {
      flagWeatherDataForecastDisplayed = false;
      displayWeatherDataCurrent(carrier, weatherDataCurrentRaw);
      
      lastButtonTouchTime = currentTime;
    }

    // Move to the current day
    if (abs(currentTime - lastButtonTouchTime) >= INACTIVITYTIMEINTERVAL && (indexDayToDisplay != 0 || flagWeatherDataForecastDisplayed == false))
    {
      indexDayToDisplay = 0;
      flagWeatherDataForecastDisplayed = true;
      displayWeatherDataForecast(carrier, weatherDataForecastParsed, indexDayToDisplay);
    }
}
