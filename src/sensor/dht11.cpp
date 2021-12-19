#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setupDHT11() {
  Serial.begin(9600);
  // Initialize device.
  dht.begin();
  Serial.println(F("DHT11 initialized"));
  // Print temperature sensor details.
}

float temprature_read(){
    float temprature= dht.readTemperature();
    if (isnan(temprature)){
        Serial.println("Failed to read temprature from sensor");
        return -1;
    }

    Serial.printf("Temprature:%f\n", temprature);
    return temprature;
}

float humidity_read(){
    float humidity=dht.readHumidity();
    if (isnan(humidity)){
        Serial.println("Failed to read humidity from sensor");
        return -1;
    }

    Serial.printf("Temprature:%f\n", humidity);
    return humidity;
}  
