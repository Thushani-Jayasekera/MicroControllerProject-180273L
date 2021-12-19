#include <Arduino.h>
#define LDRPIN 4

float lightIntensity_read(){
    float ldrVal= analogRead(LDRPIN)*100/3000;

    if(ldrVal>100){
        return 100.00;
    }
    return ldrVal;
}