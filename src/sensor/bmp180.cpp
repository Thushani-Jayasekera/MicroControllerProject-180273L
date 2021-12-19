#include <Adafruit_BMP085.h>
//I2C
#include <Wire.h>

Adafruit_BMP085 bmp180;

void setup_BMP(){
    if(!bmp180.begin()){
        Serial.println("Could not initialize BMP180 Sensor");
    }
}

float read_Pressure(){
    if(!bmp180.begin()){
        Serial.println("BMP180 Sensor Error");
        return -1;
    }

    float pressure= bmp180.readPressure();
    Serial.printf("Pressure: %f\n", pressure);
    return pressure;

}