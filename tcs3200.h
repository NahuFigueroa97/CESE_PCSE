/*
 * tcs3200.h
 *
 *  Created on: 25 may. 2021
 *      Author: nahuu
 */

#ifndef LIBS_SAPI_SAPI_V0_5_2_SOC_PERIPHERALS_INC_TCS3200_H_
#define LIBS_SAPI_SAPI_V0_5_2_SOC_PERIPHERALS_INC_TCS3200_H_

#include "sapi.h"


typedef struct{
	gpioMap_t gpio_freq;
	uint8_t   irq,
	          port,
			  pin;
}outSensor_t;

typedef struct{
	outSensor_t    data_out;
	gpioMap_t      S0,
			       S1,
			       S2,
			       S3;
	uint8_t        estado;
	uint32_t       rise_time,
	               falling_time,
			       width_time;
	uint32_t       color;
}sensorTcs3200_t;

typedef enum{
	subida,
	bajada,
	ambos
}flanco_t;

typedef enum{
	rojo,
	verde,
	azul,
	brillo
}color_t;

sensorTcs3200_t tcs3200;

void Tsc3200Init();
uint32_t colorMesure(color_t colores);
void GPIO6_IRQHandler(void);
void timerInit(tick_t tick);

#endif /* LIBS_SAPI_SAPI_V0_5_2_SOC_PERIPHERALS_INC_TCS3200_H_ */
