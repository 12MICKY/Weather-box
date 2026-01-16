#include <SoftwareSerial.h>
#include <Preferences.h>
#include "PMS.h"
PMS pms(Serial);
PMS::DATA data;

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <Wire.h>
#include <ESPAsyncWebServer.h>

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/////////////////////////////// pin setup /////////////////////////////////////////

#define TFT_CS 5
#define TFT_RST 4
#define TFT_DC 2

#define LED_COUNT 12
#define LED_PIN 27
#define swpin 12

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;

int newValues;

Preferences preferences;

/////////////////////////////////DHT setup////////////////////////////////////////////

#include "DHTesp.h"  // Click here to get the library: http://librarymanager/All#DHTesp
#include <Ticker.h>

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY !)
#error Select ESP32 board.
#endif

DHTesp dht;

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

TaskHandle_t tempTaskHandle = NULL;
Ticker tempTicker;
ComfortState cf;

bool tasksEnabled = false;
int dhtPin = 25;

/////////////////////////////////////////// variable setup //////////////////////////////////////////////////

const char *apssid = "weather0";
const char *appassword = "password1234";
const char *configPage = "/config";

int number = 0;
int mode = 0;
int x = 0;

int dangas = 14;

int pm1set = -4294967380;
int pm25set = -4294967379;
int pm10set = -4294967379;

const int wi = 41;
const int hi = 41;
// A very basic representation of a water drop
bool waterdrop[hi][wi] = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
int wifiwait = 0;

////////////////////////////// POST http for web server setup /////////////////////////////////

// String ssid = "beyond_code_campus_d";
// String password = "codingIsMySuperpower";
// const char *ssid = "ShanBing_Room_2.4G";
// const char *password = "Shangbin7";
// String ssid = "Shangbin";
// String password = "Shangbinpass";
String ssid = "";
String password = "";

//Your Domain name with URL path or IP address with path
const char *serverName = "https://wheater-station-api-web-project.shanbingpp.repl.co/report";
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 1000;


///////////////////////////// esp access point set up for entering wifi user/pass ///////////////////////////////////

WiFiServer server(80);

String header;
String submittedSSID;
String submittedPassword;
bool formSubmitted = false;

///////////////////////////// dht code /////////////////////////////////////////////

bool initTemp() {
  byte resultValue = 0;
  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");

  // Start task to get temperature
  xTaskCreatePinnedToCore(
    tempTask,        /* Function to implement the task */
    "tempTask ",     /* Name of the task */
    4000,            /* Stack size in words */
    NULL,            /* Task input parameter */
    5,               /* Priority of the task */
    &tempTaskHandle, /* Task handle. */
    1);              /* Core where the task should run */

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");
    return false;
  } else {
    // Start update of environment data every 20 seconds
    tempTicker.attach(1, triggerGetTemp);
  }
  return true;
}

void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
    xTaskResumeFromISR(tempTaskHandle);
  }
}

void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started");
  while (1)  // tempTask loop
  {
    if (tasksEnabled) {
      // Get temperature values
      TempAndHumidity newValues = dht.getTempAndHumidity();
      // Check if any reads failed and exit early (to try again).
      if (dht.getStatus() != 0) {
        Serial.println("DHT11 error status: " + String(dht.getStatusString()));
      }
      float heatIndex = dht.computeHeatIndex(newValues.temperature, newValues.humidity);
      float dewPoint = dht.computeDewPoint(newValues.temperature, newValues.humidity);
      float cr = dht.getComfortRatio(cf, newValues.temperature, newValues.humidity);
    }
    // Got sleep again
    vTaskSuspend(NULL);
  }
}

///////////////////////////////Wifi loop////////////////////////////////////////////////////////////////

float lasttemp = -100;
void httpsfunc(void *parameter) {
  delay(10000);
  while (1) {
    TempAndHumidity newValues = dht.getTempAndHumidity();
    float tempvalue = newValues.temperature;
    float humvalue = newValues.humidity;
    int index = 0;
    char value;
    char previousValue;
    int pm1;
    int pm2_5;
    int pm10;
    if (pms.read(data)) {
      Serial.print("PM 1.0 (ug/m3): ");
      Serial.println(data.PM_AE_UG_1_0);
      Serial.print("PM 2.5 (ug/m3): ");
      Serial.println(data.PM_AE_UG_2_5);
      Serial.print("PM 10.0 (ug/m3): ");
      Serial.println(data.PM_AE_UG_10_0);
      Serial.println();
    }
    if (lasttemp != tempvalue) {
      lasttemp = tempvalue;
      if (WiFi.status() == WL_CONNECTED) {
        const char *root_ca = "-----BEGIN CERTIFICATE-----\nMIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\nTzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\ncmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\nWhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\nZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\nMTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\nh77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\nA5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\nT8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\nB5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\nB5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\nKBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\nOlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\njh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\nqHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\nrU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\nHRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\nhkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\nubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\nNFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\nORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\nTkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\njNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\noyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\nmRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\nemyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n-----END CERTIFICATE-----\n";
        WiFiClientSecure client;
        HTTPClient http;
        http.setTimeout(0);
        client.setCACert(root_ca);
        // client.setInsecure();
        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName);
        // Specify content-type header
        http.addHeader("Content-Type", "application/json");
        http.addHeader("username", "shanbing");

        String httpRequestData = "{\"temp\": " + String(tempvalue) + ",\"humidity\": " + String(humvalue) + ",\"Pm\": [" + String(pm1) + "," + String(pm2_5) + "," + String(pm10) + "],\"Danger gas\": true,\"Air pressure\": 0,\"position\":[0,0],\"stationID\": 0}";
        int htl = httpRequestData.length();
        Serial.println(httpRequestData);
        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);
        String response = http.getString();
        Serial.println(response);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        // Free resources
        http.end();
      } else {
        Serial.println("WiFi Disconnected");
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

///////////////////////// void setup //////////////////////////////////////////////

int y = 0;
void setup() {
  Serial.begin(9600);
  preferences.begin("myApp", false);
  ssid = preferences.getString("key1", "default");
  password = preferences.getString("key2", "default");

  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST77XX_BLACK);

  wificonfprint();
  tft.setCursor(10, 83);
  tft.setTextSize(1);
  tft.print(password);
  tft.setCursor(10, 43);
  tft.print(ssid);
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(apssid, appassword);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
  while (true) {
    int sw1 = digitalRead(swpin);
    if (sw1 == 1) { y = 1; }
    if (y == 1 && sw1 == 0) {
      loadscreen();
      bewifi(ssid, password);
      if (wifiwait <= 96) {
        break;
      } else {
        wificonfprint();
        tft.setTextColor(ST7735_WHITE);
        tft.setCursor(10, 83);
        tft.setTextSize(1);
        tft.print(password);
        tft.setCursor(10, 43);
        tft.print(ssid);
        wifiwait = 0;
      }
      y = 0;
    }
    WiFiClient client = server.available();
    if (client) {
      Serial.println("New Client.");
      String currentLine = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          header += c;
          if (c == '\n') {
            if (currentLine.length() == 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              client.println("<html><body>");
              client.println("<h1>Wi-Fi Configuration</h1>");
              client.println("<form action='/config' method='post'>");
              client.println("SSID: <input type='text' name='ssid'><br>");
              client.println("Password: <input type='password' name='password'><br>");
              client.println("<input type='submit' value='Submit' >");
              client.println("</form></body></html>");
              client.println();
              // Break out of the while loop after sending the form
              break;
            } else {
              currentLine = "";
            }
          } else if (c != '\r') {
            currentLine += c;
          }
        }
      }

      // Check if the request is a POST request to the /config path
      if (header.indexOf("POST /config") >= 0) {
        // Read the incoming data as a form submission
        currentLine = "";
        while (client.available()) {
          char c = client.read();
          currentLine += c;
        }
        currentLine += "&";
        Serial.println(currentLine);
        submittedSSID = currentLine.substring(currentLine.indexOf('=') + 1, currentLine.indexOf('&'));
        submittedPassword = currentLine.substring(currentLine.lastIndexOf('=') + 1, currentLine.lastIndexOf('&'));
        preferences.putString("key1", submittedSSID);
        preferences.putString("key2", submittedPassword);
        submittedSSID.replace("+", " ");
        submittedPassword.replace("+", " ");
        Serial.println("Submitted SSID: " + submittedSSID);
        Serial.println("Submitted Password: " + submittedPassword);

        // Set the flag to break out of the loop
        formSubmitted = true;

        ssid = submittedSSID;
        password = submittedPassword;
        formSubmitted = true;
      }
    }
    if (formSubmitted == true) {
      //currentLine = "";
      header = "";
      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
      Serial.println("Connecting");

      wificonfprint();
      tft.setCursor(10, 83);
      tft.setTextSize(1);
      tft.print(password);
      tft.setCursor(10, 43);
      tft.print(ssid);
      formSubmitted = false;
    }
  }
  int d = 3000 / (96 - wifiwait);
  for (int i = wifiwait; i <= 96; i++) {
    tft.drawRect((tft.width() / 2) - 48, (tft.height() / 2) + 17, i, 11, ST7735_CYAN);
    delay(d);
  }
  strip.begin();
  strip.show();
  strip.setBrightness(255);
  colorWipe(strip.Color(0, 0, 0), 0);
  pinMode(swpin, INPUT);
  pinMode(dangas, INPUT);

  Serial.println("DHT ESP32 example with tasks");
  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
  xTaskCreate(httpsfunc, "Httpsfunc", 10000, NULL, 1, NULL);
}

///////////////////////// void loop //////////////////////////////////////////////

int resolution = 2.85;  // Resolution for upscaling
int scaleFactor = 100 / wi;
int posx = 45;
int posy = 25;  // 100 is the desired display size, 30 is the original array size

int maxmode = 4;
float lastvalue1 = -100;
float lastvalue2 = -100;
void loop() {
  TempAndHumidity newValues = dht.getTempAndHumidity();
  float tempvalue = newValues.temperature;
  float humvalue = newValues.humidity;
  //////////////////////////////////////////////////////////////////////////////////////////
  int sw1 = digitalRead(swpin);
  if (sw1 == 1) { x = 1; }
  if (x == 1 && sw1 == 0) {
    x = 0;
    if (mode < maxmode) {
      mode = mode + 1;
    } else if (mode >= maxmode) {
      mode = 1;
    }
    lastvalue1 = -100;
    lastvalue2 = -100;
    resettft();
  }
  if (mode == 0) {
    tft.setCursor((tft.width() / 2) - 50, (tft.height() / 2) + 40);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.print("Press to start...");
    colorWipe(strip.Color(0, 0, 0), 0);
  }
  if (mode == 1) {
    if (tempvalue <= 100) {
      int x = (tft.width() / 2) - 14;
      int y = (tft.height() / 2) + 15;
      int r = 42;
      if (lastvalue1 == -100) {
        tft.fillCircle(x, y, r, ST77XX_CYAN);
        tft.setCursor(11, 67);
        tft.setTextColor(ST77XX_BLACK);
        tft.setTextSize(3);
        tft.print(tempvalue, 1);
        tft.setCursor(15, 0);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextWrap(true);
        tft.setTextSize(3);
        tft.print("Temp");
        tft.drawCircle(x, y, r + 1, ST77XX_BLUE);
        tft.drawCircle(x, y, r + 2, ST77XX_BLUE);
        tft.drawCircle(x, y, r + 3, ST77XX_BLUE);
        tft.drawCircle(85, 65, 2, ST77XX_BLACK);
        tft.drawCircle(85, 65, 3, ST77XX_BLACK);
        tft.fillCircle(x + 59, y + 32, 14, tft.color565(255, 255, 255));
        tft.fillCircle(x + 59, 12, 7, tft.color565(255, 255, 255));
        tft.fillRect(x + 52, 14, 15, 100, tft.color565(255, 255, 255));
        tft.fillCircle(x + 59, y + 34, 9, tft.color565(255, 50, 50));
        int tempmap = map(tempvalue, 10, 0, 0, 100);
        tft.fillRect(x + 55, 13 + (100 - tempmap), 9, tempmap, tft.color565(255, 50, 50));
        tft.fillCircle(x + 59, 13 + (100 - tempmap), 4, tft.color565(255, 50, 50));
      }
      if (lastvalue1 != tempvalue) {
        tft.fillCircle(x, y, r, ST77XX_CYAN);
        tft.drawCircle(85, 65, 2, ST77XX_BLACK);
        tft.drawCircle(85, 65, 3, ST77XX_BLACK);
        tft.fillCircle(x + 59, y + 34, 14, tft.color565(255, 255, 255));
        tft.fillCircle(x + 59, 12, 7, tft.color565(255, 255, 255));
        tft.fillRect(x + 52, 14, 15, 100, tft.color565(255, 255, 255));
        tft.fillCircle(x + 59, y + 34, 9, tft.color565(255, 50, 50));

        tft.setCursor(11, 67);
        tft.setTextColor(ST77XX_BLACK);
        tft.setTextSize(3);
        tft.print(tempvalue, 1);
        int tempmap = map(tempvalue, 10, 40, 0, 100);
        tft.fillRect(x + 55, 15 + (100 - tempmap), 9, tempmap, tft.color565(255, 50, 50));
        tft.fillCircle(x + 59, 15 + (100 - tempmap), 4, tft.color565(255, 50, 50));
        lastvalue1 = tempvalue;
      }
    }
  }
  if (mode == 2) {
    if (lastvalue2 == -100) {
      resettft();
      tft.setCursor(10, 0);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextWrap(true);
      tft.setTextSize(2);
      tft.print("Humidity");
    }
    if (!isnan(humvalue)) {
      if (lastvalue2 != humvalue) {
        // Draw the upscaled image on the screen
        for (int y = map(humvalue, 0, 100, 0, hi); y >= 0; y--) {
          for (int x = 0; x < wi; ++x) {
            int pixelValue = waterdrop[y][x];

            // Draw the pixel as a 3x3 block
            for (int i = 0; i < resolution; ++i) {
              for (int j = 0; j < resolution; ++j) {
                int displayX = x * resolution + i;
                int displayY = y * resolution + j;

                // Draw the pixel on the screen
                if (pixelValue == 1) {
                  tft.drawPixel(displayX + posx, displayY + posy, ST7735_BLUE);
                } else {
                  //tft.drawPixel(displayX, displayY, ST7735_BLACK);
                }
              }
            }
          }
        }
        tft.setCursor(30, 50);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextWrap(true);
        tft.setTextSize(2);
        int inthum = humvalue;
        tft.print(inthum, 1);
        lastvalue2 = humvalue;
      }
    }
  }
  if (mode == 3) {
    int index = 0;
    char value;
    char previousValue;
    if (pms.read(data)) {
      resettft();
      tft.setCursor(0, 0);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextWrap(true);
      tft.setTextSize(1);
      tft.print("PM 1.0 (ug/m3): ");
      tft.println(data.PM_AE_UG_1_0);
      tft.print("PM 2.5 (ug/m3): ");
      tft.println(data.PM_AE_UG_2_5);
      tft.print("PM 10.0 (ug/m3): ");
      tft.println(data.PM_AE_UG_10_0);
    }
  }
  if (mode == 4) {
    resettft();
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    tft.setTextSize(1);
    tft.print("Danger gas:");
    tft.print(analogRead(dangas));
  }
  if (WiFi.status() != WL_CONNECTED) {
    resettft();
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    tft.setTextSize(4);
    tft.println("Error");
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.println("Wifi Disconnected");
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.print("Please reset box");
    while (1) {}
  }

  // if (sw1 == 1) {
  //   number = number + 1;
  // }
  // if ((number % 765) > 0 && (number % 765) < 255) {
  //   colorWipe(strip.Color(255 - ((number % 255)), (number % 255), 0), 0);
  // } else if ((number % 765) > 256 && (number % 765) < 510) {
  //   colorWipe(strip.Color(0, 255 - (number % 255), (number % 255)), 0);
  // } else if ((number % 765) > 511 && (number % 765) < 765) {
  //   colorWipe(strip.Color((number % 255), 0, 255 - (number % 255)), 0);
  // }
  delay(1000 / 20);
}

/////////////////////////////// mode /////////////////////////////////////////////

void loadscreen() {
  tft.fillScreen(tft.color565(18, 49, 102));
  tft.setCursor(20, 7);
  tft.setTextColor(ST7735_CYAN);
  tft.setTextWrap(true);
  tft.setTextSize(5);
  tft.print("The");
  tft.setCursor(15, 55);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextWrap(true);
  tft.setTextSize(2);
  tft.print("BLUE");
  tft.setTextColor(ST7735_CYAN);
  tft.print(" BOX");
  tft.drawRect((tft.width() / 2) - 52, (tft.height() / 2) + 13, 104, 19, ST7735_CYAN);
}

////////////////////////////// tft void code /////////////////////////////////////

void resettft() {
  tft.fillScreen(ST77XX_BLACK);
}
/////////////////////////////////// neopixel function ////////////////////////////////
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

/////////////////////////////// wifi begin func //////////////////////////////////////
void bewifi(String ssid, String password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && wifiwait <= 96) {
    tft.fillRect((tft.width() / 2) - 48, (tft.height() / 2) + 17, wifiwait, 11, ST7735_CYAN);
    wifiwait = wifiwait + 6;
    delay(1000);
    Serial.print(".");
    //loadtime++;
  }
  tft.fillRect((tft.width() / 2) - 48, (tft.height() / 2) + 17, wifiwait, 11, ST7735_CYAN);
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void wificonfprint() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.print("WiFi");
  tft.setCursor(53, 0);
  tft.print("Config");
  tft.setCursor(0, 20);
  tft.setTextSize(2);
  tft.print("Username");
  tft.setCursor(0, 63);
  tft.setTextSize(2);
  tft.print("Password");
}
