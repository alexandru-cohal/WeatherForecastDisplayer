#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>

#include "arduino_secrets.h"
#include "weatherData.h"

// The weather data updating interval
// 120000 ms = 2 min
// 3600000 ms = 60 min
#define UPDATETIMEINTERVAL 120000 // milliseconds

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
  while (!Serial);

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
