#define uS_TO_S_FACTOR 1000000  //faktor konverzije iz mikrosekundi u sekunde
#define TIME_TO_SLEEP  5        //period spavanja

RTC_DATA_ATTR int bootCount = 0;

void setup(){
  Serial.begin(115200);
  delay(1000);

  //inkrementacija i prikaz brojaca ciklusa budjenja
  ++bootCount;
  Serial.println("Budjenje br: " + String(bootCount));

  //prikazi razlog budjenja
  print_wakeup_reason();

  //podesavanje tajmera
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("ESP32 podesen za spavanje na " + String(TIME_TO_SLEEP) +
  " sekundi");

  //spavaj - zbog ovoga nikad nece ni uci u loop
  esp_deep_sleep_start();
}

void loop(){}

//prikaz razloga budjenja
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case 1: 
	  Serial.println("Budjenje uzrokovano spoljnim signalom koji koristi RTC_IO"); 
	  break;
    case 2: 
	  Serial.println("Budjenje uzrokovano spoljnim signalom koji koristi RTC_CNTL"); 
	  break;
    case 3: 
	  Serial.println("Budjenje uzrokovano tajmerom"); 
	  break;
    case 4: 
	  Serial.println("Budjenje uzrokovano touchpadom"); 
	  break;
    case 5: 
	  Serial.println("Budjenje uzrokovano ULP programom"); 
	  break;
    default: 
	  Serial.println("Budjenje nije uzrokovano dubokim snom"); 
	  break;
  }
}
