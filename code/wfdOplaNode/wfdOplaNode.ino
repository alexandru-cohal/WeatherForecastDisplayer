#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_JSON.h>

#include "arduino_secrets.h"

MKRIoTCarrier carrier;
WiFiClient client;
long date[7];
String weather[7];
int tempMax[7], tempMin[7], windSpeed[7];
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

  String weatherData = getWeatherData();
  parseWeatherData(weatherData);
  displayWeatherData(0);
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
      displayWeatherData(indexDayToDisplay);
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
      displayWeatherData(indexDayToDisplay);
    }
  }
}

String getWeatherData()
{
  char connectionAddress[] = "www.7timer.info";
  int connectionPort = 80;
  int DELAYWAITINGRESPONSE = 1000;
  int DELAYWAITINGNEWLINE = 100;
  
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
  String weatherData = "";
  bool flagWeatherData = false;
  
  while (client.connected()) 
  {
    currentResponseLine = client.readStringUntil('\n');

    delay(DELAYWAITINGNEWLINE);

    if (currentResponseLine == "{")
    {
      flagWeatherData = true;
    }

    if (flagWeatherData == true)
    {
      weatherData = weatherData + currentResponseLine;
    }

    if (currentResponseLine == "}") 
    {
      break;
    }
  }

  return weatherData;
}

void parseWeatherData(String weatherData)
{
  JSONVar weatherDataParsed = JSON.parse(weatherData);

  for (int day = 0; day < 7; day++)
  {
    date[day] = weatherDataParsed["dataseries"][day]["date"];
    weather[day] = JSON.stringify(weatherDataParsed["dataseries"][day]["weather"]);
    tempMax[day] = weatherDataParsed["dataseries"][day]["temp2m"]["max"];
    tempMin[day] = weatherDataParsed["dataseries"][day]["temp2m"]["min"];
    windSpeed[day] = weatherDataParsed["dataseries"][day]["wind10m_max"];
  }
}

void displayWeatherData(int dayIndex)
{
    carrier.display.fillScreen(ST77XX_BLUE);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);

    carrier.display.setCursor(20, 70);
    carrier.display.print("Date: ");
    carrier.display.print(date[dayIndex]);
    
    carrier.display.setCursor(20, 90);
    carrier.display.print("Type: ");
    carrier.display.print(weather[dayIndex]);

    carrier.display.setCursor(20, 110);
    carrier.display.print("Min temp: ");
    carrier.display.print(tempMin[dayIndex]);

    carrier.display.setCursor(20, 130);
    carrier.display.print("Max temp: ");
    carrier.display.print(tempMax[dayIndex]);

    carrier.display.setCursor(20, 150);
    carrier.display.print("Wind type: ");
    carrier.display.print(windSpeed[dayIndex]);
    
    delay(500);
}

int calculateWeekday(int day, int month, int year)
{
  // Based on the formula from: https://cs.uwaterloo.ca/~alopez-o/math-faq/node73.html
  
  int century = (year % 100) == 0 ? (year / 100) - 1 : year / 100; 
  year = (month <= 2) ? year % 100 - 1 : year % 100; 
  month = (month <= 2) ? month - 2 + 12 : month - 2;  
  
  int weekday = (day 
                 + (int)floor(2.6 * month - 0.2)
                 - 2 * century
                 + year 
                 + (int)floor(year / 4.0)
                 + (int)floor(century / 4.0)) % 7;

  return weekday;
}
