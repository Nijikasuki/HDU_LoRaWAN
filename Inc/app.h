#ifndef __APP_H
#define __APP_H
#include "stdint.h"

typedef enum {
	POWER_ON_FUNC = 0,
	DATA_REPORT_ONE_TIME_FUNC,
	DATA_REGU_REPORT_FUNC,
    NO_FUNC
} DEVICE_FUNCTION;

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


#define ATE_VERSION "V1.0.0"  //软件版本，升级时修改该信息即可
#define PRINT_ATE_VERSION_INFO(format, ...)  debug_printf("************ ---LORAWAN_NODE Development board program "format"_%s %s --- ************\r\n", ##__VA_ARGS__, __DATE__, __TIME__)

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

void lr_func_achive(void);
void lr_borad_information_print(void);














#endif 



