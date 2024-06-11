#include <atmel_start.h>
#include <stdlib.h>
#include "ftn_vip_lib/usbUART.h"
#include "ftn_vip_lib/debugUART.h"
#include "ftn_vip_lib/nbiotUART.h"
#include "ftn_vip_lib/timer_1ms.h"
#include "ftn_vip_lib/Quectel_BC68.h"

//on-board sensors:
#include "ftn_vip_lib/SHTC3.h"
#include "ftn_vip_lib/bmp280.h"
#include "ftn_vip_lib/BH1750FVI.h"
#include "ftn_vip_lib/LIS2DE12.h"

typedef struct  
{
	//SHTC3
	uint32_t shtc3_hum;
	uint32_t shtc3_temp;
	//BMP280
	int32_t bmp280_temp;
	int32_t bmp280_pres;
	//BH1750FVI
	uint32_t lum;
}sensor_data;

//parametri servera
#define ServerIP	"45.76.87.164"
#define ServerPort	50045 

void getSensorData(sensor_data *sd)
{
	char str[64];
	//SHTC3
	SHTC3_update();
	sd -> shtc3_hum = SHTC3_raw2Percent() * 100;
	sd -> shtc3_temp = SHTC3_raw2DegC() * 100;
	sprintf(str, "SHTC3 ->\tT = %d.%d C\tH = %d.%d \%\r\n", sd -> shtc3_temp / 100, sd -> shtc3_temp % 100, sd -> shtc3_hum / 100, sd -> shtc3_hum % 100);
	usbUARTputString(str);
		
	//BMP280
	bmp280_measure(&(sd -> bmp280_temp), &(sd -> bmp280_pres));
	sprintf(str, "BMP280 ->\tT = %d.%d C\tP = %d.%d mBar\r\n", sd -> bmp280_temp / 100, sd -> bmp280_temp % 100, sd -> bmp280_pres / 100, sd -> bmp280_pres % 100);
	usbUARTputString(str);
		
	//BH1750FVI
	sd -> lum = BH1750FVI_GetLightIntensity();
	sprintf(str, "BH1750FVI ->\tL = %ld lux\r\n\r\n", sd -> lum);
	usbUARTputString(str);
}

void getAccelData(accel_3axis *accel)
{
	char str[32];
	LIS2DE12_getAccel(accel);

	sprintf(str, "%d,%d,%d\r\n", accel -> accel_X, accel -> accel_Y, accel -> accel_Z);
	usbUARTputString(str);
		
	delay(5);
}

void echoTest(uint8_t protocol, char *payload)
{
	char str[128], response[128];

	//sprintf(str, "Pritisni taster za slanje...\r\n");
	//usbUARTputString(str);
	//while (gpio_get_pin_level(BUTTON));

	char socket = BC68_openSocket(1, protocol);
	int16_t rxBytes = BC68_tx(protocol, ServerIP, ServerPort, payload, strlen(payload), socket);
	BC68_rx(response, rxBytes, socket);
	sprintf(str, "Odgovor servera -> %s\r\n", response);
	usbUARTputString(str);
	BC68_closeSocket(socket);
}

int main(void)
{
	//Inicijalizacija
	atmel_start_init();		// Alat koji gasi i pali periferije - inicijalizacija
	debugUARTdriverInit();  
	usbUARTinit();
	nbiotUARTinit();
	timer_1ms_init();

	
	char str[256], payload[64]; // payload kao korisni deo poruke
	delay(3000); // Zadrska da se ukljuci terminal nakon bootloaderskog rezima
	sprintf(str, "--- FTN-VIP NB-IoT ---\r\n");
	usbUARTputString(str);
	setLEDfreq(FREQ_1HZ); // Ledovka trepce jednom u sekundi
	enableLED();

	//init sensors
	SHTC3_begin();
	bmp280_init();
	BH1750FVI_begin();
	LIS2DE12_init();
	
	//NB-IoT connect
	BC68_debugEnable(true, DEBUG_USB);
	BC68_connect();

	setLEDfreq(FREQ_1HZ);
	sensor_data sd;
	accel_3axis ad;
	while (1)
	{
		sprintf(str,"Unesite tekst poruke -> ");
		usbUARTputString(str);
		
		while(!usbUARTavailable());
		delay(20);
		usbUARTgetString(payload);	// Citanje iz serijskog bafera
		sprintf(str, "%s\r\n",payload);
		usbUARTputString(str);
		//echoTest(UDP, payload);
		echoTest(TCP, payload);
		
		//getAccelData(&ad);
		//getSensorData(&sd);
		
		//echoTest(UDP, "UDP test data...");
		//echoTest(TCP, "TCP test data...");
		
		
		//delay(5000);
	}
}

