/*============================================================================
 * Autor: Nahuel Figueroa
 * Licencia:
 * Fecha: 24/01/2021
 *===========================================================================*/

// Inlcusiones
#include "app.h"         // <= Su propia cabecera
#include "sapi.h"        // <= Biblioteca sAPI
#include "red_neuronal.h"
#include "hx711.h"
#include "tcs3200.h"

#define samples 50

#define timeColorMs 100

#define timeInOffset 3000

#define inputs 5

#define examples 6

#define capa2 3

#define capa3 3

#define outputs 3

#define numTec 2

#define tecla1 0

#define tecla2 1


typedef enum{

	entrenamiento,
	reconocimiento,

}state_t;

typedef enum{
	tangerine,
	orange,
	lemon
}fruit_t;

state_t state;

float entrada[examples][inputs] = { { 46168, 18692, 41388, 10533, 69 },
		                            { 46152, 18691, 41384, 10540, 69 },
                                    { 38333, 15220, 30683, 8234, 133 },
									{ 38300, 15209, 30676, 8224, 133 },
									{ 33182, 14315, 23852, 7231, 88 },
									{ 33187, 14305, 23832, 7234, 88 },
};

float entradaEscalada[examples][inputs] = {0};

float datosEscalados[inputs] = {0};

float salida[examples][outputs] = { { 1, 0, 0 }, { 1, 0, 0 },  { 0, 1, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0, 0, 1 } };

float y[examples][outputs] = { {0} };

float w1[inputs][capa2] = { { 0.8, 0.16, 0.5 }, { 0.2, 0.4, 0.7}, { 0.2, 0.4, 0.7}, { 0.5, 0.1, 0.8}, { 0.2, 0.1, 0.3}};

float w2[capa2][capa3] = { { 0.8, 0.16, 0.5 }, { 0.2, 0.4, 0.7 }, { 0.1, 0.4, 0.25 } };

float w3[capa3][outputs] = { { 0.8, 0.6, 0.7 }, { 0.2, 0.5, 0.1 }, { 0.1, 0.3, 0.9 } };

float u2[capa2] = { 0.16, 0.23, 0.40 };

float u3[capa3] = { 0.16, 0.23, 0.40 };

float u4[outputs] = { 0.16, 0.20, 0.11 };

float  pendiente, ordenada, offsetKiloMedio = -17881,  offsetZero = 14345, kilo_medio = 500, naranja, mandarina, limon;

uint32_t red, green, blue, luminance, count;

long weight;

uint8_t estadoActual [numTec] = {1, 1};

uint8_t estadoAnterior [numTec] = {1, 1};

bool flanco( uint8_t tecla) {

	uint8_t resultado;

	estadoActual [tecla] = gpioRead( tecla + 36) == 1 ? 0 : 1;

	resultado = estadoActual [tecla] != estadoAnterior [tecla] && estadoActual [tecla] == 1 ? 1 : 0;

	estadoAnterior [tecla] = estadoActual [tecla];

    return resultado;
}

int main(void) {

	boardConfig();
	uartInit(UART_USB, 9600);
	escaladorMatricial(entrada, entradaEscalada, examples, inputs);
	Tsc3200Init();
	hx711Init();
	powerDown();
	delay(timeInitMs);
	powerUp();
	pendiente = kilo_medio / (offsetKiloMedio - offsetZero);
	ordenada = (-1)*(kilo_medio*offsetZero) / (offsetKiloMedio - offsetZero);

	while ( TRUE) {

		switch (state) {

		case entrenamiento:

			if( flanco( tecla1 ) ){

				count = 0;
				while (count < 10) {

							backpropagation(0.20, examples, inputs, capa2, capa3, outputs, entradaEscalada, w1, w2, w3, u2, u3, u4, salida, y);

							count++;

							printf("%d \n\r",count);

							gpioWrite(LEDR,ON);

							gpioWrite(LEDG,OFF);
						}
				gpioWrite(LEDR,OFF);

			}

			if( flanco( tecla2 ) ){

              state = reconocimiento;

              gpioWrite(LEDG,ON);

              break;

				} else state = entrenamiento;

			break;

		case reconocimiento:

			red = colorMesure (rojo);

			delay(timeColorMs);

			green = colorMesure (verde);

			delay(timeColorMs);

			blue = colorMesure (azul);

			delay(timeColorMs);

			luminance = colorMesure (brillo);

			weight = pendiente*hx711Average(gainInA_128, samples) + ordenada;

			weight = weight < 0 ? 0 : weight;

			weight = weight > kilo_medio ? kilo_medio : weight;

			float datos[inputs] = {red, green, blue, luminance, weight};

			escaladorVectorial(datos, datosEscalados, inputs);

			mandarina = red_neuronal(examples, inputs, capa2, capa3, outputs, datosEscalados, w1, w2, w3, u2, u3, u4, tangerine, 0);

			naranja = red_neuronal(examples, inputs, capa2, capa3, outputs, datosEscalados, w1, w2, w3, u2, u3, u4, orange, 0);

			limon = red_neuronal(examples, inputs, capa2, capa3, outputs, datosEscalados, w1, w2, w3, u2, u3, u4, lemon, 0);

			printf("Mandarina:%.2f Naranja:%.2f Limon:%.2f \n\r", mandarina, naranja, limon);

			if( flanco( tecla1 ) ){

				state = entrenamiento;

				gpioWrite(LEDG,OFF);

				break;

			}else state = reconocimiento;

			break;

		}

		}

	return 0;
}
