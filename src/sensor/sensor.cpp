#include <Arduino.h>
#include "bmp180.h"
#include "dht11.h"
#include "ldr_mod.h"

float readTemprature(){
    return temprature_read();
}

float readHumidity(){
    return humidity_read();
}

float readPressure(){
    return read_Pressure();
}

float readLight(){
    return lightIntensity_read();
}