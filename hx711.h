/*
 * hx711.h
 *
 *  Created on: 9 jun. 2021
 *      Author: nahuu
 */

#ifndef LIBS_SAPI_SAPI_V0_5_2_SOC_PERIPHERALS_INC_HX711_H_
#define LIBS_SAPI_SAPI_V0_5_2_SOC_PERIPHERALS_INC_HX711_H_

#define timeUs 1
#define pulses 24
#define vectores 3
#define timeInitMs 10
#define INPUT 0
#define OUTPUT 1

typedef struct{
	gpioMap_t     pin_DOUT,
	              pin_PD_SCK;
	uint16_t      pulse;
	unsigned long data,
	              offset;
	long          average;
}hx711_t;

typedef enum{
	gainInA_128,
	gainInB_32,
	gainInB_64,
}gain_t;


void hx711Init() ;

void setGain(gain_t gain) ;

long hx711Read(gain_t gain);

long hx711Average(gain_t gain, uint16_t samples);

long hx711offset(gain_t gain, uint16_t samples);

void powerDown();

void powerUp();

#endif /* LIBS_SAPI_SAPI_V0_5_2_SOC_PERIPHERALS_INC_HX711_H_ */
