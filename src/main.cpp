#include <Arduino.h>
#include <WiFi.h>
#include <queue>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "config.h"
#include "utils/utils.h"

#include "cap/cap.h"
#include "http/http.h"
#include "sensor/sensor.h"
#include "sensor/bmp180.h"
#include "sensor/dht11.h"
#include "wifi/wifi.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

using namespace std;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void loadOLED()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.println("IoT MONITORING DEVICE");
  display.display();
  delay(5000);
}
String temprature_text = "";
String humidity_text = "";
String pressure_text = "";
String light_intensity_text = "";
String text4 = "";
String text5 = "";
String server_status = "";
String wifi_status = "";

void displayTextinOLED()
{
  display.setCursor(0, 0);
  display.clearDisplay();
  display.println(temprature_text);
  display.println(humidity_text);
  display.println(pressure_text);
  display.println(light_intensity_text);
  display.println(text4);
  display.println(text5);
  display.println(server_status);
  display.println(wifi_status);
  display.display();
}

//THE CACHING BUFFERS
queue<String> buffer_identifier;
queue<String> buffer_timestamp;
queue<int> buffer_msg_ids;

queue<float> buffer_temprature;
queue<float> buffer_humidity;
queue<float> buffer_pressure;
queue<float> buffer_light_intensity;

queue<float> buffer_temprature_sd;
queue<float> buffer_humidity_sd;
queue<float> buffer_pressure_sd;
queue<float> buffer_light_intensity_sd;

//initialize the variables
int msg_id = 0;
float temprature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
float light_intensity = 0.0;
float temprature_sd = 0.0;
float humidity_sd = 0.0;
float pressure_sd = 0.0;
float light_intensity_sd = 0.0;

char xml_msg_string[2000];
String identifier;
char timestamp_l[50] = {};
String timestamp;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; //5 hours and 30 minuites in seconds
const int daylightOffset_sec = 0;

void getTimestamp(char *timestamp_l)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
  }
  Serial.print("time");

  char timeString[50];
  time_t timeT = time(NULL);
  strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S%z", localtime(&timeT));
  printf("%s\n", timeString);
}

void popFromBuffers()
{
  buffer_msg_ids.pop();
  buffer_identifier.pop();
  buffer_timestamp.pop();
  buffer_temprature.pop();
  buffer_humidity.pop();
  buffer_pressure.pop();
  buffer_light_intensity.pop();
  buffer_temprature_sd.pop();
  buffer_humidity_sd.pop();
  buffer_pressure_sd.pop();
  buffer_light_intensity_sd.pop();
}

void pushToBuffers()
{
  buffer_msg_ids.push(msg_id);
  buffer_identifier.push(identifier);
  buffer_timestamp.push(timestamp);
  buffer_temprature.push(temprature);
  buffer_humidity.push(humidity);
  buffer_pressure.push(pressure);
  buffer_light_intensity.push(light_intensity);
  buffer_temprature_sd.push(temprature_sd);
  buffer_humidity_sd.push(humidity_sd);
  buffer_pressure_sd.push(pressure_sd);
  buffer_light_intensity_sd.push(light_intensity_sd);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  loadOLED();
  server_status = String("SERVER: NO");
  wifi_status = String("WIFI : NO");
  displayTextinOLED();

  //Connect to wifi
  setup_connection();
  wifi_status = String("WIFI :YES");
  displayTextinOLED();

  //initialize the sensors
  setup_BMP();
  setupDHT11();

  //find the closest ntp server in the zone
  WiFiUDP ntpUDP;
  NTPClient timeclient(ntpUDP);
  timeclient.begin();
  timeclient.setTimeOffset(gmtOffset_sec);
}

void loop()
{
  // put your main code here, to run repeatedly:

  while (!buffer_msg_ids.empty())
  {
    if (WiFi.status() == WL_CONNECTED)
    {

      wifi_status = String("WiFi: YES");
      displayTextinOLED();

      char xml_msg_string_buffer[2000];

      int msg_id_in_cache = (int)buffer_msg_ids.front();
      String identifier_in_cache = (String)buffer_identifier.front();
      String timestamp_in_cache = (String)buffer_timestamp.front();

      float temprature_in_cache = buffer_temprature.front();
      float humidity_in_cache = buffer_humidity.front();
      float pressure_in_cache = buffer_pressure.front();
      float light_intensity_in_cache = buffer_light_intensity.front();

      float temprature_sd_in_cache = buffer_temprature_sd.front();
      float humidity_sd_in_cache = buffer_humidity_sd.front();
      float pressure_sd_in_cache = buffer_pressure_sd.front();
      float light_intensity_sd_in_cache = buffer_light_intensity_sd.front();

      generate_CAP_XML_MSG(xml_msg_string_buffer, identifier_in_cache, timestamp_in_cache, temprature_in_cache, humidity_in_cache, pressure_in_cache, light_intensity_in_cache, temprature_sd_in_cache, humidity_sd_in_cache, pressure_sd_in_cache, light_intensity_sd_in_cache);

      if (has_sent_post_request(xml_msg_string_buffer, msg_id_in_cache))
      {
        //Succesfull server connection
        Serial.printf("SUCCESSFUL");
        popFromBuffers(); //remove since it is processed from the buffer
        server_status = String("SERVER:YES");
        displayTextinOLED();
      }
      else
      {
        //Connection to server failes
        server_status = String("SERVER:NO");
        displayTextinOLED();
        Serial.printf("FAILED POST REQ , %i\n\n", msg_id_in_cache);
        break;
      }
    }
    else
    {

      //IF NOT CONNECTED TO WIFI

      Serial.println("Await wifi connection..");
      wifi_status = String("WIFI:NO");
      server_status = String("SERVER:NO");
      displayTextinOLED();
      break;
    }
  }

  int x = 0;
  int n = 15;             //we need the mean of last 15 minuite data
  int round_time = 60000; //60seconds

  float temprature_reading[n]; //nth miniuite reading
  float humidity_reading[n];
  float pressure_reading[n];
  float light_intensity_reading[n];
  String edit_message_string = String("M-" + String(msg_id) + " ");

  while (x < n)
  {
    temprature_reading[x] = readPressure();
    humidity_reading[x] = readHumidity();
    pressure_reading[x] = readPressure();
    light_intensity_reading[x] = readLight();
    text5 = edit_message_string;
    displayTextinOLED();
    delay(round_time);
    x++;
  }

  identifier = String(msg_id);
  getTimestamp(timestamp_l);
  timestamp = String(timestamp_l);

  //Assign values to temprature after reading for 15 minuites
  temprature = mean(temprature_reading, n);
  humidity = mean(humidity_reading, n);
  pressure = mean(pressure_reading, n);
  light_intensity = mean(light_intensity_reading, n);

  temprature_sd = standardDeviation(temprature_reading, n, temprature);
  humidity_sd = standardDeviation(humidity_reading, n, humidity);
  pressure_sd = standardDeviation(pressure_reading, n, pressure);
  light_intensity_sd = standardDeviation(light_intensity_reading, n, light_intensity);

  temprature_text = String("Temprature" + String(temprature) + "C");
  humidity_text = String("Humidity  " + String(humidity) + "%");
  pressure_text = String("Pressure  " + String(pressure) + "Pa");
  light_intensity_text = String("Light Int " + String(light_intensity) + "%");
  displayTextinOLED();

  if (WiFi.status() == WL_CONNECTED)
  {

    // Get the xml as a string to xmlchar variable
    generate_CAP_XML_MSG(xml_msg_string, identifier, timestamp, temprature, humidity, pressure, light_intensity, temprature_sd, humidity_sd, pressure_sd, light_intensity_sd);

    if (WiFi.status() != WL_CONNECTED)
    {

      //IF WIFI IS DICONNECTED
      wifi_status = String("WIFI:NO");
      server_status = String("SERVER:YES");

      displayTextinOLED();

      //TRY RECONNECTING
      bool connected = recconect();

      if (connected)
      {

        //IF SUCCESSFULLY RECONNECTED
        wifi_status = String("WIFI:YES");
        displayTextinOLED();

        if (!has_sent_post_request(xml_msg_string, msg_id))
        {
          //IF THE TRANSMISSION OF CAP MSG IS NOT SUCCESSFUL
          pushToBuffers();
          server_status = String("SERVER:NO");
          displayTextinOLED();
        }
        else
        {
          server_status = String("SERVER:YES");
          displayTextinOLED();
        }
      }
      else
      {
        wifi_status = String("WIFI:NO");
        pushToBuffers(); //IF COULD NOT RECONNECT, PUSH THE MESSAGE DATA INTO BUFFER

        server_status = String("SERVER:NO");
        displayTextinOLED();
        connected = recconect();
      }
    }

    else if (WiFi.status() == WL_CONNECTED)
    {
      //CONNECTED TO WIFI
      wifi_status = String("WIFI:YES");
      displayTextinOLED();

      if (!has_sent_post_request(xml_msg_string, msg_id))
      {
        //IF THE TRANSMISSION OF CAP MSG IS NOT SUCCESSFUL
        pushToBuffers();

        server_status = String("SERVER:NO");
        displayTextinOLED();
      }
      else
      {
        server_status = String("SERVER:YES");
        displayTextinOLED();
      }
    }
  }
  else
  {
    wifi_status = String("WIFI:NO");
    server_status = String("SERVER:NO");
    displayTextinOLED();
  }

  msg_id++
}
