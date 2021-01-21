#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>

#include "arduino_secrets.h"

MKRIoTCarrier carrier;
WiFiClient client;

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

  char serverAddress[] = "www.7timer.info";
  
  Serial.println("\n");
  Serial.println("Starting connection to server...");
  
  if (client.connect(serverAddress, 80))
  {
    Serial.println("Connected! \n");

    // Make a HTTP request:  
    client.println("GET /bin/civillight.php?lon=14.438&lat=50.076&ac=0&unit=metric&output=json&tzshift=0 HTTP/1.1");
    client.println("Host: www.7timer.info");
    client.println("Connection: close");
    client.println();
  }
  else
  {
    Serial.println("Not connected! \n");
  } 

  delay(1000);

  String line = "";
  while (client.connected()) 
  {
    line = client.readStringUntil('\n');
    Serial.println(line);
    
    delay(100);

    if (line == "") 
    {
      break;
    }
  }
}

void loop() 
{
}
