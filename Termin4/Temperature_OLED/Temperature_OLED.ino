/**
   BasicHTTPClient.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include "heltec.h"
#include <HTTPClient.h> // For HTTP protocol
#include <ArduinoJson.h>
#define USE_SERIAL Serial

char info[20];
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
  
  Heltec.begin(true,false,true);
  Heltec.display -> setFont(ArialMT_Plain_10);
  
  wifiMulti.addAP("iPhone13mini", "kobas123"); // Define parameters for access network

}

void loop() {
  // wait for WiFi connection

  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin("https://api.openweathermap.org/data/2.5/weather?q=Novi%20Sad&appid=c5d32a3622a064c913548125fb9f6cc5&units=metric"); // Link for a current forecast, create same block just for tommorows forecast
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString(); // Sadrzaj jsona se nalazi tren u pyaloadu
        char json[1024];
        payload.toCharArray(json, payload.length() + 1); // String objekat u niz charova; +1 zbog terminacije stringa(iza poslednjeg korisnog karaktera imamo null terminator)
        DynamicJsonDocument doc(1024); // Json document
        deserializeJson(doc, json); // FJA za deserijalizaciju
        float max_temp = doc["main"]["temp_max"];
        float min_temp = doc["main"]["temp_min"];// Korisne informacije za nas
        sprintf(info,"%02f , %02f",max_temp,min_temp);
        Serial.println(max_temp);
        Serial.println(min_temp);
        Heltec.display->clear();
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->drawStringMaxWidth(0,0,128,info);
        //Heltec.display->drawStringMaxWidth(0,100,128,info);
        Heltec.display->display();
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    while(digitalRead(0)); // Nakon pritisnutog tastera PRD - kad nije pritisnut onda je 1
  }

   
  //delay(5000); // Ponovo preuzimanje stranice nakon 5sekundi
}
