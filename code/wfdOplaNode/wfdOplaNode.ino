#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Arduino_JSON.h>

#include "arduino_secrets.h"

MKRIoTCarrier carrier;
WiFiClient client;
long date[7];
String weather[7];
int tempMax[7], tempMin[7], windSpeed[7];

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
}

void loop() 
{
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
