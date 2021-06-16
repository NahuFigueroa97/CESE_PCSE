/*
 * red_neuronal.c
 *
 *  Created on: 17 mar. 2021
 *      Author: Nahuel Figueroa
 */

#include "red_neuronal.h"
#include "math.h"
#include "stdio.h"

float suma = 0;
int i, j, k, p, e, r, s, t, u, v;

float sigmoide(double x) {
	float val;
	val = 1 / (1 + exp(-x));
	return val;
}

float red_neuronal(int examples, int capa_1, int capa_2, int capa_3, int capa_4,
		float *entrada, float *w1, float *w2, float *w3, float *u2, float *u3,
		float *u4, int i_capa_4, int i_example) {
	suma = 0;
	for (k = 0; k < capa_3; k++) {
		suma = suma
				+ a_3(examples, capa_1, capa_2, capa_3, &entrada[0], &w1[0],
						&w2[0], &u2[0], &u3[0], k, i_example)
						* (*(w3 + (capa_3 * i_capa_4) + k));
	}

	return sigmoide(suma + (*(u4 + i_capa_4)));

}

float a_3(int examples, int capa_1, int capa_2, int capa_3, float *entrada,
		float *w1, float *w2, float *u2, float *u3, int i_capa_3, int i_example) {
	suma = 0;
	for (j = 0; j < capa_2; j++) {

		suma = suma
				+ a_2(examples, capa_1, capa_2, &entrada[0], &w1[0], &u2[0], j,
						i_example) * (*(w2 + (i_capa_3 + (capa_2 * j))));

	}

	return sigmoide(suma + (*(u3 + i_capa_3)));

}

float a_2(int examples, int capa_1, int capa_2, float *entrada, float *w1,
		float *u2, int i_capa_2, int i_example) {
	suma = 0;
	for (i = 0; i < capa_1; i++) {

		suma = suma
				+ (*(entrada + (i_example * capa_1) + i))
						* (*(w1 + (i_capa_2 + (capa_1 * i))));
	}

	return sigmoide(suma + (*(u2 + i_capa_2)));
}

float delta_4(int examples, int capa_4, float *Y, float *S, int i_example,
		int i_capa_4) {
	return (*(Y + (i_example * capa_4 + i_capa_4)))
			* (1 - (*(Y + (i_example * capa_4 + i_capa_4))))
			* ((*(Y + (i_example * capa_4 + i_capa_4)))
					- (*(S + (i_example * capa_4 + i_capa_4))));
}

float delta_3(int examples, int capa_1, int capa_2, int capa_3, int capa_4,
		float *entrada, float *w1, float *w2, float *w3, float *Y, float *S,
		float *u2, float *u3, int i_capa_1, int i_capa_2, int i_capa_3,
		int i_capa_4, int i_example) {
	return a_3(examples, capa_1, capa_2, capa_3, &entrada[0], &w1[0], &w2[0],
			&u2[0], &u3[0], i_capa_3, i_example)
			* (1
					- a_3(examples, capa_1, capa_2, capa_3, &entrada[0], &w1[0],
							&w2[0], &u2[0], &u3[0], i_capa_3, i_example))
			* (*(w3 + (i_capa_3 * capa_4) + i_capa_4))
			* delta_4(examples, capa_4, Y, S, i_example, i_capa_4);
}

float delta_2(int examples, int capa_1, int capa_2, int capa_3, int capa_4,
		float *entrada, float *w1, float *w2, float *w3, float *Y, float *S,
		float *u2, float *u3, int i_capa_1, int i_capa_2, int i_capa_3,
		int i_capa_4, int i_example) {
	return a_2(examples, capa_1, capa_2, &entrada[0], &w1[0], &u2[0], i_capa_2,
			i_example)
			* (1
					- a_2(examples, capa_1, capa_2, &entrada[0], &w1[0], &u2[0],
							i_capa_2, i_example))
			* delta_3(examples, capa_1, capa_2, capa_3, capa_4, &entrada[0],
					&w1[0], &w2[0], &w3[0], &Y[0], &S[0], &u2[0], &u3[0],
					i_capa_1, i_capa_2, i_capa_3, i_capa_4, i_example);
}

void backpropagation(float alfa, int examples, int capa_1,
		int capa_2, int capa_3, int capa_4, float *entrada, float *w1,
		float *w2, float *w3, float *u2, float *u3, float *u4, float *S,
		float *Y) {
	alfa = 0.20;

	for (r = 0; r < examples; r++) {
		for (s = 0; s < capa_1; s++) {
			for (t = 0; t < capa_2; t++) {
				for (u = 0; u < capa_3; u++) {
					for (v = 0; v < capa_4; v++) {
						*(Y + (r * capa_4) + v) = red_neuronal(examples, capa_1,
								capa_2, capa_3, capa_4, &entrada[0], &w1[0],
								&w2[0], &w3[0], &u2[0], &u3[0], &u4[0], v, r);
						(*(w3 + (u * capa_4) + v)) = (*(w3 + (u * capa_4) + v))
								- alfa
										* a_3(examples, capa_1, capa_2, capa_3,
												&entrada[0], &w1[0], &w2[0],
												&u2[0], &u3[0], u, r)
										* delta_4(examples, capa_4, Y, S, r, v);
						(*(u4 + v)) = (*(u4 + v))
								- alfa * delta_4(examples, capa_4, Y, S, r, v);

						(*(w2 + (t * capa_3) + u)) = (*(w2 + (t * capa_3) + u))
								- alfa
										* a_2(examples, capa_1, capa_2,
												&entrada[0], &w1[0], &u2[0], t,
												r)
										* delta_3(examples, capa_1, capa_2,
												capa_3, capa_4, &entrada[0],
												&w1[0], &w2[0], &w3[0], &Y[0],
												&S[0], &u2[0], &u3[0], s, t, u,
												v, r);
						(*(u3 + u)) = (*(u3 + u))
								- alfa
										* delta_3(examples, capa_1, capa_2,
												capa_3, capa_4, &entrada[0],
												&w1[0], &w2[0], &w3[0], &Y[0],
												&S[0], &u2[0], &u3[0], s, t, u,
												v, r);

						(*(w1 + (s * capa_2) + t)) = (*(w1 + (s * capa_2) + t))
								- alfa * (*entrada + (r * capa_1) + s)
										* delta_2(examples, capa_1, capa_2,
												capa_3, capa_4, &entrada[0],
												&w1[0], &w2[0], &w3[0], &Y[0],
												&S[0], &u2[0], &u3[0], s, t, u,
												v, r);
						(*(u2 + t)) = (*(u2 + t))
								- alfa
										* delta_2(examples, capa_1, capa_2,
												capa_3, capa_4, &entrada[0],
												&w1[0], &w2[0], &w3[0], &Y[0],
												&S[0], &u2[0], &u3[0], s, t, u,
												v, r);
					}
				}
			}
		}
	}
}

float maximoMatricial(float *matriz_entrenamiento, uint8_t sizeX, uint8_t sizeY) {
	int indexX, indexY;
	float max = 0;
	for (indexX = 0; indexX < sizeX; indexX++) {
		for (indexY = 0; indexY < sizeY; indexY++) {
			if ( (*(matriz_entrenamiento + (indexX * sizeY) + indexY)) > max ) {
				max = (*(matriz_entrenamiento + (indexX * sizeY) + indexY));
			}
		}
	}
	return max;
}

float minimoMatricial(float *matriz_entrenamiento, uint8_t sizeX, uint8_t sizeY) {
	int indexX, indexY;
	float min = 1000;
	for (indexX = 0; indexX < sizeX; indexX++) {
		for (indexY = 0; indexY < sizeY; indexY++) {
			if (min > (*(matriz_entrenamiento + (indexX * sizeY) + indexY))) {
				min = (*(matriz_entrenamiento + (indexX * sizeY) + indexY));
			}
		}
	}
	return min;
}

float minimoVectorial(float *matriz_entrenamiento, uint8_t size) {
	int index;
	float min = 1000;
		for (index = 0; index < size; index++) {
			if (min > (*(matriz_entrenamiento + index ) )) {
				min = (*(matriz_entrenamiento + index ));
			}
	}
	return min;
}

float maximoVectorial(float *matriz_entrenamiento, uint8_t size) {
	int index;
	float max = 0;
		for (index = 0; index < size; index++) {
			if ((*(matriz_entrenamiento + index ) > max )) {
				max = (*(matriz_entrenamiento + index ));
			}
	}
	return max;
}

void escaladorMatricial(float *matriz_entrenamiento, float *matriz_escalada,uint8_t sizeX, uint8_t sizeY) {
	int indexX, indexY;
	float max, min;
	max = maximoMatricial(&matriz_entrenamiento[0], sizeX, sizeY);
	min = minimoMatricial(&matriz_entrenamiento[0], sizeX, sizeY);
	for (indexX = 0; indexX < sizeX; indexX++) {
		for (indexY = 0; indexY < sizeY; indexY++) {
	       *(matriz_escalada + (indexX * sizeY) + indexY) = ((*(matriz_entrenamiento + (indexX * sizeY) + indexY)) - min) / (max - min);
		}
	}
}

void escaladorVectorial(float *matriz_entrenamiento, float *matriz_escalada,uint8_t size) {
	int index;
	float max, min;
	max = maximoVectorial(&matriz_entrenamiento[0], size);
	min = minimoVectorial(&matriz_entrenamiento[0], size);
	for (index = 0; index < size; index++) {
	       *(matriz_escalada + index) = (*(matriz_entrenamiento + index) - min) / (max - min);
	}
}

