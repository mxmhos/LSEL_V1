/*
 * cliente.h
 *
 *  Created on: 21 de enero de 2019
 *      Author: FFM
 */

#ifndef _CLIENTE_H_
#define _CLIENTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#include "piTankGoLib.h"
#include "torreta.h"

extern int flags_player;
extern TipoPosicionTorreta next_move;

// Prototipos de funciones de entrada
int ComprueboServer ();


#endif /* _CLIENTE_H_ */
