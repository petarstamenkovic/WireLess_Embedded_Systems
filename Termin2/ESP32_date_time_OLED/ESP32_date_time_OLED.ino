/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  Modified on : 27.3.2023. by Petar Stamenkovic
  This code syncs live time for NTP server and lets user set an alarm time. ESP then goes to deep sleep in order to save energy and wakes up at the time user set.
*/

#include <WiFi.h>
#include "time.h"
#include "heltec.h"
#include <string.h>
#define uS_TO_S_FACTOR 1000000 // Converting factor us to s

// Your wifi information
const char* ssid     = "********";
const char* password = "********";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// Variables for an alarm time
int hrs_a;
int min_a;
int sec_a;
char alarm_time[15];
int time_to_wake;

void setup() {
  Serial.begin(115200);

  Serial.println("Good morning gorgeous!");

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  // Init and OLED
  Heltec.begin(true, false, false);
  Heltec.display->setFont(ArialMT_Plain_10);

  // Set up an alarm time
  Serial.println("Enter the time to wake up!");

  Serial.println("Hours : ");
  while (Serial.available() == 0) {}
  hrs_a = Serial.parseInt();
  Serial.read();  // Serial.read in these statments is used to consume the newline character so you can input multiple data(h,m,s)

  Serial.println("Minutes : ");
  while (Serial.available() == 0) {}
  min_a = Serial.parseInt();
  Serial.read();
  
  Serial.println("Seconds: ");
  while (Serial.available() == 0) {}
  sec_a = Serial.parseInt();
  Serial.read();
  
  sprintf(alarm_time, "%02d:%02d:%02d", hrs_a, min_a, sec_a); // Convert the alarm info into string for better printing manipulation
  Serial.println("Alarm time: ");
  Serial.println(alarm_time);
  Serial.println("Good night Irene!");

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  time_to_wake = calculateWakeUp();   // Calculate the amount of time for ESP to wake up, see function below
  Serial.println("Alarm should go of in : " + String(time_to_wake) + " seconds!");
  
  esp_sleep_enable_timer_wakeup(time_to_wake * uS_TO_S_FACTOR); // Configure the wakeup reason - timer

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  esp_deep_sleep_start(); // Enter deep sleep
}


// Loop block will never be reached in this regime so its not neccesary
void loop() {
  Serial.println("Hi im in loop block, good morning!");
  delay(1000);
  printLocalTime();
}

// Function that calculates the amount of time for ESP to sleep, fetch the current time from NTP server and subtract it from an alarm time you set
// Convert both in seconds since the wake up function requires time in seconds
int calculateWakeUp()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) 
  {
    Serial.println("Failed to obtain time");
    return -1;
  }

  int curr_time_secs = timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
  int alarm_time_secs = hrs_a * 3600 + min_a * 60 + sec_a;
  return alarm_time_secs - curr_time_secs;
}

// Function that prints local tie from NTP server, uncomment statments below for additional time information
void printLocalTime() 
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) 
  {
    Serial.println("Failed to obtain time");
    return;
  }

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  /*
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);

  */
  char curr_time[9];
  sprintf(curr_time, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->drawStringMaxWidth(0, 0, 128, curr_time);
  Heltec.display->display();

  time_t now;
  time(&now);
  //Serial.print("UNIX TS: ");
  //Serial.println(now);

  Serial.println();
}
