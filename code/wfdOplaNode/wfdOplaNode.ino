#include <WiFiNINA.h>

#include "arduino_secrets.h"

void setup() 
{
  // put your setup code here, to run once:

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
}

void loop() {
  // put your main code here, to run repeatedly:

}
