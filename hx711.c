/*
 * HX711.c
 *
 *  Created on: 9 jun. 2021
 *      Author: nahuel figueroa
 */

#include "sapi.h"
#include "math.h"
#include "hx711.h"
#include "stdio.h"
#include "debouncing.h"

hx711_t hx711;

void hx711Init() {
	hx711.pin_DOUT = GPIO1;
	hx711.pin_PD_SCK = GPIO2;
	hx711.pulse = pulses/vectores;
	hx711.data = 0;
	hx711.average = 0;
	hx711.offset = 0;
	gpioInit(hx711.pin_DOUT, INPUT);
	gpioInit(hx711.pin_PD_SCK, OUTPUT);
}

void setGain(gain_t gain) {
	switch (gain) {
	case gainInA_128:
		gpioWrite(hx711.pin_PD_SCK, ON);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, OFF);
		delayInaccurateUs(timeUs);
		break;
	case gainInB_32:
		gpioWrite(hx711.pin_PD_SCK, ON);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, OFF);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, ON);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, OFF);
		delayInaccurateUs(timeUs);
		break;
	case gainInB_64:
		gpioWrite(hx711.pin_PD_SCK, ON);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, OFF);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, ON);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, OFF);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, ON);
		delayInaccurateUs(timeUs);
		gpioWrite(hx711.pin_PD_SCK, OFF);
		delayInaccurateUs(timeUs);
		break;
	}
}

long hx711Read(gain_t gain) {

	hx711.data = 0;
	uint8_t value[3] = {0};
	uint8_t filter = 0x00;

	for (uint8_t i = 0; i < hx711.pulse; i++) {

		gpioWrite(hx711.pin_PD_SCK, ON);

		value[2] |= gpioRead(hx711.pin_DOUT) << (hx711.pulse-1-i);

		gpioWrite(hx711.pin_PD_SCK, OFF);

	}

	for (uint8_t i = 0; i < hx711.pulse; i++) {

		gpioWrite(hx711.pin_PD_SCK, ON);

		value[1] |= gpioRead(hx711.pin_DOUT) << (hx711.pulse-1-i);

		gpioWrite(hx711.pin_PD_SCK, OFF);

	}

	for (uint8_t i = 0; i < hx711.pulse; i++) {

		gpioWrite(hx711.pin_PD_SCK, ON);

		value[0] |= gpioRead(hx711.pin_DOUT) << (hx711.pulse-1-i);

		gpioWrite(hx711.pin_PD_SCK, OFF);

	}

	setGain(gain);

	filter = value[2] & 0x80 ? 0xFF : 0X00;

	hx711.data = ( (unsigned long)( filter )   << 24 |
			       (unsigned long)( value[2] ) << 16 |
				   (unsigned long)( value[1] ) << 8  |
				   (unsigned long)( value[0] ) );


	return (long) (hx711.data);

}

long hx711Average(gain_t gain, uint16_t samples) {

	hx711.average = 0;

	for (uint8_t i = 0; i < samples; i++) {

		hx711.average = hx711.average + hx711Read(gain);
		delay(timeInitMs);

	}

	hx711.average = hx711.average / samples;

	return hx711.average;
}

long hx711offset(gain_t gain, uint16_t samples) {

	uint8_t flag = gpioRead(TEC1) ;

		while ( flag ) {
		hx711.offset = hx711Average(gain, samples);
		gpioWrite(LEDR, ON);
		gpioWrite(LEDG, OFF);
		flag = gpioRead(TEC1);
		}
		gpioWrite(LEDR, OFF);
		gpioWrite(LEDG, ON);

	return hx711.offset = hx711Average(gain, samples);

}

void powerDown(){
	gpioWrite(hx711.pin_PD_SCK, OFF);
	gpioWrite(hx711.pin_PD_SCK, ON);
}

void powerUp(){
	gpioWrite(hx711.pin_PD_SCK, OFF);
}

