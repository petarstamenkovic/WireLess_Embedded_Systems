/**
   BasicHTTPClient.ino

    Created on: 24.05.2015
    Modified on : 27.3.2023. by Petar Stamenkovic 
    This code switches beetwen todays and tommorows forecast by pressing a PRD button on a LoRa board(LoRa 32 Heltec V2) using a 
    openweather API -> https://openweathermap.org/

*/

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include "heltec.h"
#include <HTTPClient.h> // For HTTP protocol
#include <ArduinoJson.h>
#define USE_SERIAL Serial

char max_temp_s[20];
char min_temp_s[20];
char humidity_s[20];
char date_t_s[20];
char message_max[40] = "Max temp : ";
char message_min[40] = "Min temp : ";
char message_hum[40] = "Humidity : ";
char message_max_t[40] = "Max temp : ";
char message_min_t[40] = "Min temp : ";
char message_hum_t[40] = "Humidity : ";
char message_date_t[40] = "Date : " ;


WiFiMulti wifiMulti;

/*
  const char* ca = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n" \
  "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
  "DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \
  "SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n" \
  "GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
  "AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n" \
  "q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n" \
  "SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n" \
  "Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n" \
  "a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n" \
  "/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n" \
  "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n" \
  "CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n" \
  "bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n" \
  "c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n" \
  "VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n" \
  "ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n" \
  "MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n" \
  "Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n" \
  "AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n" \
  "uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n" \
  "wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n" \
  "X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n" \
  "PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n" \
  "KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n" \
  "-----END CERTIFICATE-----\n";
*/

void setup() {

  USE_SERIAL.begin(115200);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  // OLED screen init
  Heltec.begin(true, false, true);
  Heltec.display -> setFont(ArialMT_Plain_10);

  // Define your network SSID and PASSWORD
  wifiMulti.addAP("******", "*******");

}

void loop() {
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.print("[HTTP] begin : Fetching info for todays forecast...\n");
    http.begin("https://api.openweathermap.org/data/2.5/weather?q=Novi%20Sad&appid=c5d32a3622a064c913548125fb9f6cc5&units=metric"); // Link for a current forecast, create same block just for tommorows forecast
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString(); // Load JSON information in a String object payload
        char json[1024];
        payload.toCharArray(json, payload.length() + 1); // Convert a string object into a char array, +1 is for the final, null terminator
        DynamicJsonDocument doc(1024); // Create a dynamic JSON document
        deserializeJson(doc, json); // Function that deserializes a JSON into a doc

        // Extracting usefull information for us and converting them into a strings in order to print them on OLED Display,here is shown only min,max temp and humidity
        // but you can add any from JSON doc the same way i did here.
        float max_temp = doc["main"]["temp_max"];
        float min_temp = doc["main"]["temp_min"];
        int humidity = doc["main"]["humidity"];
        sprintf(max_temp_s, "%.2f", max_temp);
        sprintf(min_temp_s, "%.2f", min_temp);;
        sprintf(humidity_s, "%d", humidity);

        // Tried this, not compatible because of the loop nature, strings keep on concatinating every iteration!
        //strcat(message_max, max_temp_s);
        //strcat(message_min, min_temp_s);
        //strcat(message_hum, humidity_s);

        // Serial monitor print just to see if information is valid, this is optional
        Serial.println(max_temp);
        Serial.println(min_temp);
        Serial.println(humidity);

        // Display information on OLED display
        Heltec.display->clear();
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->drawString(0, 0, "Todays forecast");
        Heltec.display->drawString(0, 10, message_max); Heltec.display->drawString(50, 10, max_temp_s);
        Heltec.display->drawString(0, 20, message_min); Heltec.display->drawString(50, 20, min_temp_s);
        Heltec.display->drawString(0, 30, message_hum); Heltec.display->drawString(50, 30, humidity_s);
        Heltec.display->display();
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    while (digitalRead(0)); // Wait for a PRD button press and move on with code
    ///////// CODE FOR A TOMMOROWS FORECAST ///////////// 
    // This code is almost similar to the previous block - changed the http link mostly // 
    Serial.print("[HTTP begin : Fetching info for tommorows forecast...\n]");
    http.begin("https://api.openweathermap.org/data/2.5/forecast?q=Novi%20Sad&appid=c5d32a3622a064c913548125fb9f6cc5&units=metric&cnt=9"); // Link for a tommrows forecast
    Serial.print("[HTTP] GET...\n");
    httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK)
      {
        //Serial.println("Code is OK so im in the if Statment");
        String payload_t = http.getString(); 
        //Serial.println(payload_t);
        char json_t[4200];                       // Caution here, initial JSON doc from this link is huge(around 27k chars), we just fetch first 4200 because its enough.
        payload_t.toCharArray(json_t, 4199 + 1); // Load only 4200 chars, if you encounter some issues, reduce the size of this, or pay for a better API DOC on this link : https://openweathermap.org/forecast16
        //Serial.println(json_t);
        DynamicJsonDocument doc_t(1024);
        deserializeJson(doc_t, json_t);

        // Exctracting information from JSON, this one is "multi-level" so we access it differently. Index 8 is for tommorows forecast at 9AM
        String date_t = doc_t["list"][8]["dt_txt"];
        date_t.toCharArray(date_t_s, 10 + 1); // Fetch only first 10 chars from date (mm.dd.yyyy hh.mm --> mm.dd.yyyy)
        float max_temp_t = doc_t["list"][8]["main"]["temp_max"];
        float min_temp_t = doc_t["list"][8]["main"]["temp_min"];
        int humidity_t = doc_t["list"][8]["main"]["humidity"];
        sprintf(max_temp_s, "%.2f", max_temp_t);
        sprintf(min_temp_s, "%.2f", min_temp_t);
        sprintf(humidity_s, "%d", humidity_t);

        // Same issue as mentioned up
        //strcat(message_date_t, date_t_s);
        //strcat(message_max_t, max_temp_s);
        //strcat(message_min_t, min_temp_s);
        //strcat(message_hum_t, humidity_s);

        // Console printing
        Serial.println(date_t);
        Serial.println(date_t_s);
        Serial.println(max_temp_t);
        Serial.println(min_temp_t);
        Serial.println(humidity_t);

        // Display info on OLED
        Heltec.display->clear();
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->drawString(0, 0, message_date_t); Heltec.display->drawString(30, 0, date_t_s);
        Heltec.display->drawString(0, 10, message_max_t); Heltec.display->drawString(50, 10, max_temp_s);
        Heltec.display->drawString(0, 20, message_min_t); Heltec.display->drawString(50, 20, min_temp_s);
        Heltec.display->drawString(0, 30, message_hum_t); Heltec.display->drawString(50, 30, humidity_s);
        Heltec.display->display();
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    while (digitalRead(0)); // Wait for a button press and after it clear the OLED as you go in another iteration
    Heltec.display->clear();
  }
}
