#ifndef _KEY_H
#define _KEY_H

#include "stm32l4xx_hal.h"
#include "app.h"

#define A_K1 GPIO_PIN_13
#define A_K2 GPIO_PIN_6

typedef struct
{
	uint8_t A_KEY1;
	uint8_t A_KEY2;
	uint8_t A_KEY1_KEY2;
	uint8_t B_KEY1;
	uint8_t B_KEY2;
	uint8_t B_KEY3;
	uint8_t B_KEY4;
	uint8_t B_KEY5;
}KEY;

void KEY_DO(KEY *key_temp,DEVICE_SATE *Device_Sate_temp);

#endif
