#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

bool has_sent_post_request(char *xml_msg_string , int msg_id){
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("Connection lost!");
        return false;
    }

    //if connection is not lost,
    HTTPClient httpClient;
    String dataURL=(String)server+"/data";
    httpClient.begin((String)dataURL);
    int response= httpClient.POST(xml_msg_string);
    httpClient.end();

    //failed response
    if(response<=-1){
        String error=httpClient.errorToString(response).c_str();
        Serial.println(error.c_str());
        return false;
    }
    //NOT 200 response code
    if(response!= HTTP_CODE_OK){
        Serial.println(response);
        return false;
    }

    //successfull respinse-- 200 res
    Serial.println("200 OK");
    return true;
}