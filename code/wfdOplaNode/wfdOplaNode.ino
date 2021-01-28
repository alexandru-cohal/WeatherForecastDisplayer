#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>

#include "arduino_secrets.h"
#include "weatherData.h"

MKRIoTCarrier carrier;
weatherData wd;
int indexDayToDisplay = 0;

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

  //Initialize the MKR IoT Carrier and output any errors in the serial monitor
  CARRIER_CASE = true;
  carrier.begin();
  carrier.display.setRotation(0);

  String weatherDataRaw = getWeatherDataRaw();
  parseWeatherDataRaw(weatherDataRaw, wd);
  displayWeatherData(carrier, wd, 0);
}

void loop() 
{ 
  carrier.Buttons.update();
  
  if (carrier.Button0.onTouchDown()) 
  {
    indexDayToDisplay--;
    if (indexDayToDisplay < 0)
    {
      indexDayToDisplay = 0;
    }
    else
    {
      displayWeatherData(carrier, wd, indexDayToDisplay);
    }
  }

  if (carrier.Button4.onTouchDown()) 
  {
    indexDayToDisplay++;
    if (indexDayToDisplay > 6)
    {
      indexDayToDisplay = 6;
    }
    else
    {
      displayWeatherData(carrier, wd, indexDayToDisplay);
    }
  }
}
