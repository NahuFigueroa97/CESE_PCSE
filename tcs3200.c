/*
 * tcs3200.c
 *
 *  Created on: 25 may. 2021
 *      Author: nahuu
 */
#include "sapi.h"
#include "tcs3200.h"

#define output 1
#define input 0
#define irqGpio 6
#define port_gpio 3
#define pin_gpio 6
#define tiempo 6000000
#define tiempo_color 100
#define PRIORITY 5
#define timeus Timer_microsecondsToTicks(tiempo)

sensorTcs3200_t tcs3200={
		{GPIO6, irqGpio, port_gpio, pin_gpio} , GPIO8, GPIO7, GPIO5, GPIO3, rojo, 0, 0, 0, 0
};

static void GPIOIrq(uint8_t irq, uint8_t port, uint8_t pin, flanco_t edge) {

	/*Habilito interrupcion*/
	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	/*Selección de pin de interrupción GPIO*/
	Chip_SCU_GPIOIntPinSel(irq, port, pin);
	/* Borrar el estado de configuracion actual de la interrupción */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(irq));
	/* Establece el modo de interrupción por flanco*/
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(irq));

	if (edge == subida) {
		/* Habilita interrupcion en GPIO por flanco de subida */
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(irq));
	} else if (edge == bajada) {
		/* Habilita interrupcion en GPIO por flanco de bajada*/
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(irq));
	} else {
		/* Habilita interrupcion en GPIO por ambos flancos */
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(irq));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(irq));
	}

	/* Limpia las interrupciones pendientes en irq */
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn + irq);
	/* Habilita interrupciones en irq */
	NVIC_EnableIRQ(PIN_INT0_IRQn + irq);
}

void GPIO6_IRQHandler(void) {

	/* Si la interrupcion fue por flanco de subida */
	if (Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH(tcs3200.data_out.irq)) {
		/* Captura el tiempo en el que ocurrio el flanco de subida */
		tcs3200.rise_time = Chip_TIMER_ReadCount(LPC_TIMER0);
		/* Limpia interrupcion de flanco de subida */
		Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT,PININTCH(tcs3200.data_out.irq));
	} else {
		/* Captura el tiempo en el que ocurrio el flanco de bajada */
		tcs3200.falling_time = Chip_TIMER_ReadCount(LPC_TIMER0);
		/* Calcula la duracion del pulso */
		tcs3200.width_time = tcs3200.falling_time - tcs3200.rise_time;
		/* Limpia interrupcion de flanco de bajada */
		Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT,PININTCH(tcs3200.data_out.irq));
	}

	/* Limpia estado de la interrupcion */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT,PININTCH(tcs3200.data_out.irq));

}

void timerInit(tick_t tick) {

	/* Habilita clock de timer y su reset */
	Chip_TIMER_Init(LPC_TIMER0);
	Chip_RGU_TriggerReset(RGU_TIMER0_RST);
	while (Chip_RGU_InReset(RGU_TIMER0_RST)) {
	}
	Chip_TIMER_Reset(LPC_TIMER0);

	/* Inicializar comparar coincidencias con los ticks especificados (número de recuentos necesarios para borrar el contador de coincidencias) */
	Chip_TIMER_MatchEnableInt( LPC_TIMER0, TIMERCOMPAREMATCH0);
	Chip_TIMER_SetMatch( LPC_TIMER0, TIMERCOMPAREMATCH0, tick);

	/* Makes Timer Match 0 period the timer period*/
	Chip_TIMER_ResetOnMatchEnable( LPC_TIMER0, TIMERCOMPAREMATCH0);

	/*Habilita timer*/
	Chip_TIMER_Enable(LPC_TIMER0);

	/* Habilita interrupciones de timer */
	NVIC_SetPriority(TIMER0_IRQn, PRIORITY + 1);
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_ClearPendingIRQ(TIMER0_IRQn);

}

void Tsc3200Init() {

	gpioConfig(tcs3200.S0, output);
	gpioConfig(tcs3200.S1, output);
	gpioConfig(tcs3200.S2, output);
	gpioConfig(tcs3200.S3, output);
	gpioConfig(tcs3200.data_out.gpio_freq, input);

	gpioWrite(tcs3200.S0, ON); /*20% DE FRECUENCIA MAXIMA*/
	gpioWrite(tcs3200.S1, OFF);

	/*Habilito ambos bordes porque necesito calcular el ancho del pulso*/
	GPIOIrq(tcs3200.data_out.irq, tcs3200.data_out.port, tcs3200.data_out.pin, ambos);

	timerInit(timeus);

}

uint32_t colorMesure(color_t colores) {
	switch (colores) {
	case rojo:
		gpioWrite(tcs3200.S2, OFF);
		gpioWrite(tcs3200.S3, OFF);
		tcs3200.color = tcs3200.width_time;
		break;
	case azul:
		gpioWrite(tcs3200.S2, OFF);
		gpioWrite(tcs3200.S3, ON);
		tcs3200.color = tcs3200.width_time;
		break;
	case verde:
		gpioWrite(tcs3200.S2, ON);
		gpioWrite(tcs3200.S3, ON);
		tcs3200.color = tcs3200.width_time;
		break;
	case brillo:
		gpioWrite(tcs3200.S2, ON);
		gpioWrite(tcs3200.S3, OFF);
		tcs3200.color = tcs3200.width_time;
		break;

	}

	return tcs3200.color;

}
