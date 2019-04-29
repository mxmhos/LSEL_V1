/*
 * torreta.h
 *
 *  Created on: 21 de enero de 2019
 *      Author: FFM
 */

#ifndef _TORRETA_H_
#define _TORRETA_H_

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include "piTankGoLib.h"

typedef struct {
	int inicio; // Valor correspondiente a la posicion inicial del servo
	int incremento; // Cuantía en que se incrementa el valor de la posicion con cada movimiento del servo
	int minimo; // Valor mínimo correspondiente a la posicion del servo
	int maximo; // Valor maximo correspondiente a la posicion del servo
} TipoServo;

typedef struct {
	int x; // Coordenada x correspondiente a la posicion del servo horizontal
	int y; // Coordenada y correspondiente a la posicion del servo vertical
} TipoPosicionTorreta;

typedef struct {
	TipoPosicionTorreta posicion;
	TipoServo servo_x;
	TipoServo servo_y;
} TipoTorreta;

extern TipoPosicionTorreta next_move;

// Prototipos de procedimientos de inicializacion de los objetos especificos
int InicializaTorreta (TipoTorreta *p_torreta);
void ComprueboTorreta();


#endif /* _TORRETA_H_ */
