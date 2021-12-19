#include <Arduino.h>

void generate_CAP_XML_MSG(char *xml_msg_string, String identifier, String timestamp,float temprature, float humidity, float pressure, float light_intensity,float temprature_sd, float humidity_sd, float pressure_sd, float light_sd ){

    String temprature_string= String(temprature);
    String humidity_string= String(humidity);
    String pressure_string= String(pressure);
    String light_intensity_string= String(light_intensity);

    String temprature_sd_string= String(temprature_sd);
    String humidity_sd_string= String(humidity_sd);
    String pressure_sd_string= String(pressure_sd);
    String light_sd_intensity_string= String(light_sd);
    //CAP XML msg
    sprintf(
        xml_msg_string,

        "<?xml version = \"1.0\" encoding = \"UTF-8\"?>"
        "<alert xmlns = \"urn:oasis:names:tc:emergency:cap:1.2\">"
        "<identifier>%s</identifier>" 
        "<sender>IOT@COLOMBO</sender>"
        "<sent>%s</sent>"
        "<status>Actual</status>"
        "<msgType>Update</msgType>"
        "<scope>Public</scope>"
        "<info>"
        "<category>Met</category>"
        "<event>ENVIROMENT READINGS</event>"
        "<responseType>Shelter</responseType>"
        "<urgency>Immediate</urgency>"
        "<severity>Minor</severity>"
        "<certainty>Observed</certainty>"

        "<senderName>ENVIROMENT MONITORING DEVICE OF THUSHANI</senderName>"
        "<headline>READINGS</headline>"

        "<description> MEASURED TEMPRATURE, HUMIDITY, PRESSURE AND AMBIENT LIGHT LEVEL IN A CITY OF COLOMBO AREA</description>"

        "<parameter>"
        "<valueName>temprature</valueName>"
        "<value>%s</value>"
        "</parameter>"

        "<parameter>"
        "<valueName>humidity</valueName>"
        "<value>%s</value>"
        "</parameter>"

        "<parameter>"
        "<valueName>pressure</valueName>"
        "<value>%s</value>"
        "</parameter>"

        "<parameter>"
        "<valueName>lightIntensity</valueName>"
        "<value>%s</value>"
        "</parameter>"


        "<area>"
        "<areaDesc>36, WENNAWATTE, WELLAMPITIYA, COLOMBO, WESTERN, SRI LANKA</areaDesc>"


        "</area>"
        "</info>"
        "</alert>",


        identifier.c_str(),
        timestamp.c_str(),
        temprature_string.c_str(),
        humidity_string.c_str(),
        pressure_string.c_str(),
        light_intensity_string.c_str(),
        temprature_sd_string.c_str(),
        humidity_sd_string.c_str(),
        pressure_sd_string.c_str(),
        light_sd_intensity_string.c_str()


);
}


