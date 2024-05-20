#include "SPIFFS.h"
#include <WiFi.h>
#include <WebServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "PametnomDosta";
const char* password = "Nal1vPer0";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Error initializing SPIFFS");
    return;
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
    
  // Route for root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/komplet.html", "text/html");
  });

  // Route for image
  server.on("/komplet.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/komplet.jpg", "image/jpeg");
  });

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  
}
