#include <Arduino.h>
#include <WiFi.h>
#include "../config.h"

void setup_connection(){
    Serial.begin(115200);
    WiFi.begin(ssid,password);
    
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println('Connecting');
        delay(1000);
    }
    Serial.println(WiFi.localIP());


}

bool recconect(){
   WiFi.begin(ssid,password);
   if (WiFi.status()!= WL_CONNECTED){
       delay(900);
       Serial.print("reconnecting..");
       for(;;); // Don't proceed, loop forever
       return false;
   }

   Serial.print("Connected to WIFI!");
   return true;
}