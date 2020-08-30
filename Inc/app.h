#ifndef __APP_H
#define __APP_H
#include "stdint.h"

typedef enum {
	START_MODE = 0,
	CMD_MODE,
	TRANSPARENT_MODE,
    LPTSEND_MODE,
    TIMSEND_MODE,
	TESTAUTO_MODE,     
} DEVICE_SATE;

/*---传感器相关参数定义---*/
//typedef struct {
//	uint8_t Led;		//0 , 1
//	uint32_t Pressure;	//Hpa/10
//	int16_t Altitude;	//m*10
//	int16_t Tempter;	//?*100
//	int16_t Humidi;		//%RH
//	uint16_t Lux;		//lux*100
//	ACCELER_T acc;		//g*100, x,y,z
//	uint8_t battery;	//0 for extern, 1~254 level
//	GPS_T	gps;		//longitude, latitude
//	int16_t reserve1;
//	int16_t reserve2;
//	int16_t reserve3;
//} SENSOR;

typedef struct {
	char upcnt[10];
	char ackcnt[10];
	char toa[10];
	char nbt[10];
	
	char ch[10];
	char sf[10];
	char pwr[10];
	char per[10];
	
	char rssi[10];
	char snr[10];
	
} DEBUG;

void User_App(void);















#endif 



