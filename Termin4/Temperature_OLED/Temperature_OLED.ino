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

  Heltec.begin(true, false, true);
  Heltec.display -> setFont(ArialMT_Plain_10);

  wifiMulti.addAP("iPhone13mini", "kobas123"); // Define parameters for access network

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
        String payload = http.getString(); // Sadrzaj jsona se nalazi tren u pyaloadu
        char json[1024];
        payload.toCharArray(json, payload.length() + 1); // String objekat u niz charova; +1 zbog terminacije stringa(iza poslednjeg korisnog karaktera imamo null terminator)
        DynamicJsonDocument doc(1024); // Json document
        deserializeJson(doc, json); // FJA za deserijalizaciju

        // Korisne informacije za nas i njihov uspis u string
        float max_temp = doc["main"]["temp_max"];
        float min_temp = doc["main"]["temp_min"];
        int humidity = doc["main"]["humidity"];
        sprintf(max_temp_s, "%.2f", max_temp);
        sprintf(min_temp_s, "%.2f", min_temp);;
        sprintf(humidity_s, "%d", humidity);

        // Spajanje poruke i ekstrahovanog podatka
        //strcat(message_max, max_temp_s);
        //strcat(message_min, min_temp_s);
        //strcat(message_hum, humidity_s);

        // Ispis na konzolu
        Serial.println(max_temp);
        Serial.println(min_temp);
        Serial.println(humidity);

        // Ispis na OLED display
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
    while (digitalRead(0)); // Wait for a PRD button press
    ///////// CODE FOR A TOMMOROWS FORECAST /////////////
    Serial.print("[HTTP begin : Fetching info for tommorows forecast...\n]");
    http.begin("https://api.openweathermap.org/data/2.5/forecast?q=Novi%20Sad&appid=c5d32a3622a064c913548125fb9f6cc5&units=metric"); // Link for a tommrows forecast
    Serial.print("[HTTP] GET...\n");
    httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK)
      {
        //Serial.println("Code is OK so im in the if Statment");
        String payload_t = http.getString(); // Sadrzaj jsona se nalazi tren u pyaloadu
        //Serial.println(payload_t);
        char json_t[1024];                       // CEO JSON DOKUMENT IMA 27k karaktera!
        payload_t.toCharArray(json_t, 1023 + 1); // ? 1024 karaktera je dovoljno samo za jedan element liste iz jsona a meni treba 8. indeks iz iste za sutrasnji dan, kako to kad bi mi onda trebalo tipa char[10k]
        //Serial.println(json_t);
        DynamicJsonDocument doc_t(1024); // Json document
        deserializeJson(doc_t, json_t); // FJA za deserijalizaciju

        // Ekstrakcija podataka iz json dokumenta - 9ti element liste je sutrasnji dan u 9 - Zbog dileme sa memorijom stoji indeks 0, tolko moze da se ucita
        String date_t = doc_t["list"][0]["dt_txt"];
        date_t.toCharArray(date_t_s, 10 + 1); // Skrati date format, ne treba nam vreme
        float max_temp_t = doc_t["list"][0]["main"]["temp_max"];
        float min_temp_t = doc_t["list"][0]["main"]["temp_min"];
        int humidity_t = doc_t["list"][0]["main"]["humidity"];
        sprintf(max_temp_s, "%.2f", max_temp_t);
        sprintf(min_temp_s, "%.2f", min_temp_t);
        sprintf(humidity_s, "%d", humidity_t);

        // Spajanje poruke i ekstrahovanog podatka - Ne radi jer ovaj kod ide u loop i konstantno nalepljuje nove info
        //strcat(message_date_t, date_t_s);
        //strcat(message_max_t, max_temp_s);
        //strcat(message_min_t, min_temp_s);
        //strcat(message_hum_t, humidity_s);

        // Ispis na konzolu
        Serial.println(date_t);
        Serial.println(date_t_s);
        Serial.println(max_temp_t);
        Serial.println(min_temp_t);
        Serial.println(humidity_t);

        // Ispis na OLED
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
    while (digitalRead(0)); // Nakon pritisnutog tastera PRD - kad nije pritisnut onda je 1
    Heltec.display->clear();
  }

  //delay(5000); // Ponovo preuzimanje stranice nakon 5sekundi
}
