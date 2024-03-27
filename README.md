Laboratory tasks from the master year course "Umrezeni Embedded Sistemi".
The course covers coding using LoRa 32 Heltec V2 board and mostly Arduino IDE for wireless data logging. 

Some of these are plain laboratory examples that can be found in Arduino IDE example block, but the ones i modifed and tested are following so far : 

1. Termin 2 -> ESP32_date_time_OLED (In this task user sets an alarm time and sends ESP into deep sleep in order to save energy and wakes up at the alarm. Time sync is done via NTP server.)
2. Termin 4 -> Temperature_OLED (In this task I use a openWeather API to fetch weather information for today and tommorow. By pressing PRD button on LoRa Board, user can switch beetwen today and tommrow and print that info on OLED display.)
